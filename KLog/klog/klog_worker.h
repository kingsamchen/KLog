/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KLOG_KLOG_WORKER_H_
#define KLOG_KLOG_WORKER_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "klog/basic_macros.h"
#include "klog/basic_types.h"

namespace klog {

class LogWorker {
public:
    LogWorker(FileNameGenerator gen, std::chrono::seconds flush_interval, size_t roll_size,
              std::chrono::hours roll_interval);

    ~LogWorker();

    DISALLOW_COPY(LogWorker);

    DISALLOW_MOVE(LogWorker);

    void Send(std::string&& message);

private:
    void WorkFunc();

private:
    using Buffer = std::vector<char>;

private:
    FileNameGenerator file_name_gen_;
    const std::chrono::seconds flush_interval_;
    const size_t roll_size_;
    const std::chrono::hours roll_interval_;
    Buffer working_buffer_;
    Buffer backlog_buffer_;
    std::atomic<bool> done_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;
};

}   // namespace klog

#endif  // KLOG_KLOG_WORKER_H_