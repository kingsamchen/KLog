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
#include <memory>
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
    // TODO: Try to use fixed-buffer rather than normal std::vector to implement Buffer.
    using Buffer = std::vector<char>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;

private:
    FileNameGenerator file_name_gen_;
    const std::chrono::seconds flush_interval_;
    const size_t roll_size_;
    const std::chrono::hours roll_interval_;
    std::unique_ptr<Buffer> working_buffer_;
    std::unique_ptr<Buffer> backlog_buffer_;
    BufferVector buffer_bridge_;
    std::atomic<bool> done_;
    std::mutex mutex_;
    std::condition_variable bridge_not_empty_cond_;
    std::thread thread_;
};

}   // namespace klog

#endif  // KLOG_KLOG_WORKER_H_