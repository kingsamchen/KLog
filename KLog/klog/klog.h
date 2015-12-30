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

LogSeverity GetMinSeverityLevel();

void SetMinSeverityLevel(LogSeverity min_severity);

void ConfigureLogger(LogWorker* logger);

// Surprisingly, a macro `ERROR` is defined as 0 in file <wingdi.h>, which is
// included by <windows.h>, so we add a special macro to handle this peculiar
// chaos, in case the file was included.
#define COMPACT_LOG_INFO \
    klog::LogMessage(__FILE__, __LINE__, klog::LogSeverity::LOG_INFO)
#define COMPACT_LOG_WARNING \
    klog::LogMessage(__FILE__, __LINE__, klog::LogSeverity::LOG_WARNING)
#define COMPACT_LOG_ERROR \
    klog::LogMessage(__FILE__, __LINE__, klog::LogSeverity::LOG_ERROR)
#define COMPACT_LOG_0 \
    klog::LogMessage(__FILE__, __LINE__, klog::LogSeverity::LOG_ERROR)
#define COMPACT_LOG_FATAL \
    klog::LogMessage(__FILE__, __LINE__, klog::LogSeverity::LOG_FATAL)

#define LOG_IS_ON(severity) \
    ((klog::LogSeverity::LOG_##severity) >= klog::GetMinSeverityLevel())
#define LOG_STREAM(severity) \
    COMPACT_LOG_##severity.stream()
#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : klog::LogMessageVoidfy() & (stream)

#define LOG(severity) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

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

// Used to suppress compiler warning or intellisense error.
struct LogMessageVoidfy {
    void operator&(const std::ostream&) const
    {}
};

}   // namespace klog

#endif  // KLOG_KLOG_H_