/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KLOG_KLOG_H_
#define KLOG_KLOG_H_

#include <sstream>

#include "klog/basic_macros.h"

namespace klog {

class LogWorker;

// These log severities will be used as index into the array `kLogSeverityNames`.
enum class LogSeverity : int {
    LOG_INFO = 0,
    LOG_WARNING,
    LOG_ERROR,
#if defined(OS_WIN)
    LOG_0 = LOG_ERROR,  // For why, see `COMPACT_LOG_0`.
#endif
    LOG_FATAL
};

void SetMinSeverityLevel(LogSeverity min_severity);

void ConfigureLogger(LogWorker* logger);

// TODO: add convenient macros.

class LogMessage {
public:
    LogMessage(const char* file, int line, LogSeverity severity_level);

    ~LogMessage();

    DISALLOW_COPY(LogMessage);

    DISALLOW_MOVE(LogMessage);

    std::ostream& stream()
    {
        return stream_;
    }

private:
    const char* file_;
    int line_;
    LogSeverity severity_;
    std::ostringstream stream_;
};

}   // namespace klog

#endif  // KLOG_KLOG_H_