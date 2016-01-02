/*
 @ 0xCCCCCCCC
*/

#include <conio.h>

#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include <windows.h>

#include "klog/klog.h"
#include "klog/klog_worker.h"

namespace chrono = std::chrono;

void TestWorker()
{
    klog::LogWorker worker([] { return PATH_LITERAL("test.log"); }, std::chrono::seconds(3), 10 * 1024 * 1024,
                           std::chrono::hours(24));
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<> dist(100, 1000);
    int count = 10;
    for (auto i = 0; i < count; ++i) {
        std::ostringstream ss;
        ss << "This is the " << i << "th trial\n";
        worker.Send(ss.str());
        ss.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(engine)));
    }
}

void TestLogMessageHeaderConstruction()
{
    klog::LogMessage message(__FILE__, __LINE__, klog::LogSeverity::LOG_INFO);
    std::cout << static_cast<std::ostringstream&>(message.stream()).str();
}

void TestLogIsOnMacro()
{
    std::cout << LOG_IS_ON(INFO) << std::endl
              << LOG_IS_ON(ERROR) << std::endl;
    klog::SetMinSeverityLevel(klog::LogSeverity::LOG_ERROR);
    std::cout << "min level changed to LOG_ERROR" << std::endl;
    std::cout << LOG_IS_ON(INFO) << std::endl
              << LOG_IS_ON(WARNING) << std::endl
              << LOG_IS_ON(ERROR) << std::endl;
}

void TestLogMacro()
{
    LOG(INFO) << "logging on INFO level";
    LOG(WARNING) << "logging on WARNING level";
    LOG(ERROR) << "logging on ERROR level";
    LOG(FATAL) << "logging on FATAL level";

    // Simulate that <windows.h> was included.
#ifndef ERROR
#define ERROR 0
#endif
    klog::SetMinSeverityLevel(klog::LogSeverity::LOG_ERROR);
    std::cout << "changed min level to LOG_ERROR\n";

    LOG(INFO) << "logging on INFO level";
    LOG(WARNING) << "logging on WARNING level";
    LOG(ERROR) << "logging on ERROR level";
    LOG(FATAL) << "logging on FATAL level";
#undef ERROR
    LOG_IF(ERROR, 1 == 1) << "conditional logging";
    LOG_IF(ERROR, 2 == 1) << "conditional logging";
}

void TestWhole()
{
    auto file_gen = []() {
        tm time_now;
        time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        localtime_s(&time_now, &now);
        klog::PathChar timestamp[20];
        wcsftime(timestamp, 20, L"%Y%m%d%H%M%S", &time_now);

        DWORD pid = GetCurrentProcessId();

        klog::PathString file_name(timestamp);
        file_name.append(L"_").append(std::to_wstring(pid)).append(L"_test.log");

        return file_name;
    };

    klog::LogWorker worker(file_gen, chrono::seconds(3), 10 * 1024 * 1024, chrono::hours(24));
    klog::ConfigureLogger(&worker);

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<> dist(100, 1000);
    int count = 10;
    for (auto i = 0; i < count; ++i) {
        std::ostringstream ss;
        ss << "This is the " << i << "th trial";
        LOG(INFO) << ss.str();
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(engine)));
    }

    std::cout << "-eof-" << std::endl;
}

int main()
{
    TestWhole();
    _getch();
    return 0;
}