/*
@ 0xCCCCCCCC
*/

#include "klog/klog_worker.h"

namespace {

const size_t kDefaultBufferSize = 4 * 1024 * 1024;

}   // namespace

namespace klog {

LogWorker::LogWorker(const PathString& log_file_path, std::chrono::seconds flush_interval)
    : log_file_path_(log_file_path), flush_interval_(flush_interval), done_(false)
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

        // TODO: write to disk.
        relay_buffer.clear();
    }
}

}   // namespace klog