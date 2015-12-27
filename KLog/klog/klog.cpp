/*
 @ 0xCCCCCCCC
*/

#include "klog/klog.h"

#include "klog/klog_worker.h"

namespace {

using klog::LogWorker;
using klog::LogSeverity;

const char* kLogSeverityNames[] { "INFO", "WARNING", "ERROR", "FATAL" };

LogSeverity g_min_severity_level = LogSeverity::LOG_INFO;
LogWorker* g_log_worker = nullptr;

}   // namespace

namespace klog {

void SetMinSeverityLevel(LogSeverity min_severity)
{
    g_min_severity_level = min_severity;
}

void ConfigureLogger(LogWorker* logger)
{
    g_log_worker = logger;
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity_level)
    : file_(file), line_(line), severity_(severity_level)
{}

LogMessage::~LogMessage()
{}

}   // namespace klog