/*
 @ 0xCCCCCCCC
*/

#include "klog/klog_file.h"

#include <codecvt>
#include <locale>

namespace {

inline std::string AsUTF8(const std::string& str)
{
    return str;
}

inline std::string AsUTF8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(str);
}

}   // namespace

namespace klog {

class LogFileException : public std::runtime_error {
public:
    explicit LogFileException(const std::string& message)
        : runtime_error(message)
    {}

    explicit LogFileException(const char* message)
        : runtime_error(message)
    {}
};

LogFile::LogFile(const PathString& file_path)
    : file_(file_path, std::ios::app)
{
    if (!file_) {
        throw LogFileException(std::string("Failed to open the file ") + AsUTF8(file_path));
    }
}

void LogFile::Append(const char* data, size_t data_size)
{
    file_.write(data, data_size);
    // TODO: roll the file in some strategies.
}

void LogFile::Flush()
{
    file_.flush();
}

}   // namespace klog