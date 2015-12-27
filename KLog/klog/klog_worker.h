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
    // TODO: add rollsize for logfile.
    LogWorker(const PathString& log_file_path, std::chrono::seconds flush_interval);

    ~LogWorker();

    DISALLOW_COPY(LogWorker);

    DISALLOW_MOVE(LogWorker);

    void Send(std::string&& message);

private:
    void WorkFunc();

private:
    using Buffer = std::vector<char>;

private:
    PathString log_file_path_;
    std::chrono::seconds flush_interval_;
    Buffer working_buffer_;
    Buffer backlog_buffer_;
    std::atomic<bool> done_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;
};

}   // namespace klog

#endif  // KLOG_KLOG_WORKER_H_