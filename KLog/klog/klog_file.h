/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KLOG_KLOG_FILE_H_
#define KLOG_KLOG_FILE_H_

#include <fstream>

#include "klog/basic_macros.h"
#include "klog/basic_types.h"

namespace klog {

class LogFileException;

class LogFile {
public:
    explicit LogFile(const PathString& file_path);

    ~LogFile() = default;

    DISALLOW_COPY(LogFile);

    DISALLOW_MOVE(LogFile);

    void Append(const char* data, size_t data_size);

    void Flush();

private:
    // TODO: We need to roll the log file when some situations satisfied.
    void Roll();

private:
    PathString file_path_;
    std::ofstream file_;
};

}   // namespace klog

#endif  // KLOG_KLOG_FILE_H_
