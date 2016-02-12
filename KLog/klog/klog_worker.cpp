/*
 @ 0xCCCCCCCC
*/

#include "klog/klog_worker.h"

#include <cassert>

#include "klog/klog_file.h"

namespace {

namespace chrono = std::chrono;

const size_t kBufferSize = 4 * 1024 * 1024;

}   // namespace

namespace klog {

LogWorker::LogWorker(FileNameGenerator gen, chrono::seconds flush_interval, size_t roll_size,
                     chrono::hours roll_interval)
    : file_name_gen_(gen), flush_interval_(flush_interval), roll_size_(roll_size),
      roll_interval_(roll_interval), working_buffer_(new Buffer()), backlog_buffer_(new Buffer()),
      done_(false)
{
    working_buffer_->reserve(kBufferSize);
    backlog_buffer_->reserve(kBufferSize);
    thread_ = std::thread(&LogWorker::WorkFunc, this);
}

LogWorker::~LogWorker()
{
    done_.store(true, std::memory_order::memory_order_release);
    thread_.join();
}

void LogWorker::Send(std::string&& message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (working_buffer_->capacity() - working_buffer_->size() > message.size()) {
        working_buffer_->insert(working_buffer_->end(), message.begin(), message.end());
    } else {
        buffer_bridge_.push_back(std::move(working_buffer_));

        // If backlog buffer is not available, usually when writing speed of front-end is
        // extremely faster than flusing speed of back-end, we create another buffer at hand.
        if (backlog_buffer_) {
            working_buffer_ = std::move(backlog_buffer_);
        } else {
            working_buffer_.reset(new Buffer());
            working_buffer_->reserve(kBufferSize);
        }

        working_buffer_->insert(working_buffer_->end(), message.begin(), message.end());
        bridge_not_empty_cond_.notify_one();
    }
}

void LogWorker::WorkFunc()
{
    auto relay_working_buffer = std::make_unique<Buffer>();
    relay_working_buffer->reserve(kBufferSize);
    auto relay_backlog_buffer = std::make_unique<Buffer>();
    relay_backlog_buffer->reserve(kBufferSize);
    BufferVector buffer_to_write;

    LogFile log_file(file_name_gen_, roll_size_, roll_interval_);
    while (!done_.load(std::memory_order::memory_order_acquire)) {
        assert(relay_working_buffer &&
               relay_working_buffer->empty() &&
               relay_working_buffer->capacity() == kBufferSize);
        assert(relay_backlog_buffer &&
               relay_backlog_buffer->empty() &&
               relay_backlog_buffer->capacity() == kBufferSize);
        assert(buffer_to_write.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            // If there is no buffer in the bridge, get a sleep until either
            // being signaled or a timeout is reached.
            if (buffer_bridge_.empty()) {
                bridge_not_empty_cond_.wait_for(lock, flush_interval_);
            }

            buffer_bridge_.push_back(std::move(working_buffer_));
            working_buffer_ = std::move(relay_working_buffer);
            if (!backlog_buffer_) {
                backlog_buffer_ = std::move(relay_backlog_buffer);
            }

            using std::swap;
            swap(buffer_bridge_, buffer_to_write);
        }

        // TODO: Try to drop excessive buffers.

        for (const auto& buffer : buffer_to_write) {
            if (!buffer->empty()) {
                log_file.Append(buffer->data(), buffer->size());
            }
        }

        if (buffer_to_write.size() > 2) {
            buffer_to_write.resize(2);
        }

        if (!relay_working_buffer) {
            relay_working_buffer = std::move(buffer_to_write.back());
            relay_working_buffer->clear();
            buffer_to_write.pop_back();
        }

        if (!relay_backlog_buffer) {
            relay_backlog_buffer = std::move(buffer_to_write.back());
            relay_backlog_buffer->clear();
            buffer_to_write.pop_back();
        }

        buffer_to_write.clear();
    }
}

}   // namespace klog