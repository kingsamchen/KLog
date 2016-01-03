/*
 @ 0xCCCCCCCC
*/

#include "klog/klog_worker.h"

#include "klog/klog_file.h"

namespace {

namespace chrono = std::chrono;

const size_t kDefaultBufferSize = 4 * 1024 * 1024;

}   // namespace

namespace klog {

LogWorker::LogWorker(FileNameGenerator gen, chrono::seconds flush_interval, size_t roll_size,
                     chrono::hours roll_interval)
    : file_name_gen_(gen), flush_interval_(flush_interval), roll_size_(roll_size),
      roll_interval_(roll_interval), done_(false)
{
    working_buffer_.reserve(kDefaultBufferSize);
    backlog_buffer_.reserve(kDefaultBufferSize);
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
    if (working_buffer_.capacity() - working_buffer_.size() > message.size()) {
        working_buffer_.insert(working_buffer_.end(), message.begin(), message.end());
    } else {
        using std::swap;
        swap(working_buffer_, backlog_buffer_);
        working_buffer_.insert(working_buffer_.end(), message.begin(), message.end());
        cond_.notify_one();
    }
}

void LogWorker::WorkFunc()
{
    Buffer relay_buffer;
    relay_buffer.reserve(kDefaultBufferSize);
    LogFile log_file(file_name_gen_, roll_size_, roll_interval_);
    while (!done_.load(std::memory_order::memory_order_acquire)) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // May be waked up due to either being signaled or tiemout.
            cond_.wait_for(lock, flush_interval_);
            using std::swap;
            if (backlog_buffer_.empty()) {
                swap(working_buffer_, relay_buffer);
            } else {
                swap(backlog_buffer_, relay_buffer);
            }
        }

        if (relay_buffer.empty()) {
            continue;
        }

        log_file.Append(relay_buffer.data(), relay_buffer.size());
        relay_buffer.clear();
    }
}

}   // namespace klog