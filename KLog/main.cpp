/*
 @ 0xCCCCCCCC
*/

#include <conio.h>

#include <random>
#include <sstream>

#include "klog/klog_worker.h"

void WorkerTest()
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

int main()
{
    _getch();
    return 0;
}