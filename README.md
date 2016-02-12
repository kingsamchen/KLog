# KLOG

### Introduction

A light-weight asynchronous logger using C++ 11.

KLog supports log file rolling according to some given strategies, that why you are required to provide a method for generating log file name.

### Compatibility

Compilers that support C++ 11.

It should also be noted that, there are few secured-CRT functions only available in Visual Studio used, I plan to replace them with portable wrappers in the future.

### Usages and Samples

``` c++
int main()
{
    auto file_gen = []() {
        tm time_now;
        time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        localtime_s(&time_now, &now);
        klog::PathChar timestamp[20];
        wcsftime(timestamp, 20, PAHT_LITERAL("%Y%m%d%H%M%S"), &time_now);

        DWORD pid = GetCurrentProcessId();

        klog::PathString file_name(timestamp);
        file_name.append(PAHT_LITERAL"_").append(std::to_wstring(pid)).append(PAHT_LITERAL"_test.log");

        return file_name;
    };

    klog::LogWorker worker(file_gen, chrono::seconds(3), 10 * 1024 * 1024,
                           chrono::duration_cast<chrono::hours>(chrono::seconds(3)));
    klog::ConfigureLogger(&worker);
  
    // maybe in another thread
    LOG(INF) << "blahblah";
    LOG(ERROR) << "what-the-hell?";
}
```