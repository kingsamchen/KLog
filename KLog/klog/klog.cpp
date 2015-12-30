/*
 @ 0xCCCCCCCC
*/

#include "klog/klog.h"

#include <iomanip>
#include <thread>

#include "klog/klog_worker.h"

namespace {

using klog::LogWorker;
using klog::LogSeverity;

const char* kLogSeverityNames[] {"INFO", "WARNING", "ERROR", "FATAL"};

LogSeverity g_min_severity_level = LogSeverity::LOG_INFO;
LogWorker* g_log_worker = nullptr;

void OutputNowTimestamp(std::ostream& stream)
{
    namespace chrono = std::chrono;

    // Because c-style date&time utilities don't support microsecond precison,
    // we have to handle it on our own.
    auto time_now = chrono::system_clock::now();
    auto duration_in_us = chrono::duration_cast<chrono::microseconds>(time_now.time_since_epoch());
    auto us_part = duration_in_us - chrono::duration_cast<chrono::seconds>(duration_in_us);

    tm local_time_now;
    time_t raw_time = chrono::system_clock::to_time_t(time_now);
    _localtime64_s(&local_time_now, &raw_time);
    stream << std::put_time(&local_time_now, "%Y%m%d %H:%M:%S.")
           << us_part.count();
}

const char* ExtractFileName(const char* file_path)
{
    const char* p = file_path;
    const char* last_pos = nullptr;
    for (; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            last_pos = p;
        }
    }

    return last_pos ? last_pos + 1 : file_path;
}

template<typename E>
constexpr auto EnumToUnderlying(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

}   // namespace

namespace klog {

LogSeverity GetMinSeverityLevel()
{
    return g_min_severity_level;
}

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
{
    stream_ << "[";
    OutputNowTimestamp(stream_);
    stream_ << " " << std::this_thread::get_id()
            << " " << kLogSeverityNames[EnumToUnderlying(severity_level)]
            << " " << ExtractFileName(file) << "(" << line << ")]";
}

LogMessage::~LogMessage()
{
    // TODO: Should we log stack trace for LOG_FATAL?
    stream_ << "\n";
    g_log_worker->Send(stream_.str());
}

}   // namespace klog