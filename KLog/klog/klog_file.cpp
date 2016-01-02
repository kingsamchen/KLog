/*
 @ 0xCCCCCCCC
*/

#include "klog/klog_file.h"

#include <codecvt>
#include <locale>

namespace {

namespace chrono = std::chrono;

const int kCheckForAppendCount = 10;

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

LogFile::LogFile(FileNameGenerator gen, size_t roll_size, chrono::hours roll_interval)
    : file_name_gen_(gen),
      file_path_(gen()),
      file_(file_path_, std::ios::app),
      roll_size_(roll_size),
      roll_interval_(roll_interval),
      written_size_(0),
      check_count_(0),
      start_time_point_(chrono::system_clock::now())
{
    if (!file_) {
        throw LogFileException(std::string("Failed to open the file ") + AsUTF8(file_path_));
    }
}

void LogFile::Append(const char* data, size_t data_size)
{
    file_.write(data, data_size);
    file_.flush();

    written_size_ += data_size;
    if (written_size_ >= roll_size_) {
        RollFile();
        return;
    }

    ++check_count_;
    if (check_count_ >= kCheckForAppendCount) {
        check_count_ = 0;
        auto duration = chrono::system_clock::now() - start_time_point_;
        if (duration >= roll_interval_) {
            RollFile();
        }
    }
}

void LogFile::RollFile()
{
    file_.close();

    file_path_ = file_name_gen_();
    file_.open(file_path_, std::ios::app);
    if (!file_) {
        throw LogFileException(std::string("Failed to open the file ") + AsUTF8(file_path_));
    }

    written_size_ = 0;
    start_time_point_ = chrono::system_clock::now();
}

}   // namespace klog