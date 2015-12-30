/*
 @ 0xCCCCCCCC
*/

#include <conio.h>

#include <iostream>
#include <random>
#include <sstream>

#include "klog/klog.h"
#include "klog/klog_worker.h"

void TestWorker()
{
    klog::LogWorker worker(std::wstring(L"test.log"), std::chrono::seconds(3));
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

int main()
{
    TestLogMacro();
    _getch();
    return 0;
}