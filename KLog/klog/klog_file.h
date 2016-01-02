/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KLOG_KLOG_FILE_H_
#define KLOG_KLOG_FILE_H_

#include <chrono>
#include <fstream>

#include "klog/basic_macros.h"
#include "klog/basic_types.h"

namespace klog {

class LogFileException;

class LogFile {
public:
    explicit LogFile(FileNameGenerator gen, size_t roll_size, std::chrono::hours roll_interval);

    ~LogFile() = default;

    DISALLOW_COPY(LogFile);

    DISALLOW_MOVE(LogFile);

    void Append(const char* data, size_t data_size);

private:
    void RollFile();

private:
    FileNameGenerator file_name_gen_;
    PathString file_path_;
    std::ofstream file_;
    const size_t roll_size_;
    const std::chrono::hours roll_interval_;
    size_t written_size_;
    int check_count_;
    std::chrono::time_point<std::chrono::system_clock> start_time_point_;
};

}   // namespace klog

#endif  // KLOG_KLOG_FILE_H_
