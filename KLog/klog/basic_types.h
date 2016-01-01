/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KLOG_BASIC_TYPES_H_
#define KLOG_BASIC_TYPES_H_

#include <string>

#include "klog/basic_macros.h"

namespace klog {

#if defined(OS_WIN)
using PathChar = wchar_t;
using PathString = std::wstring;
#else
using PathChar = char;
using PathString = std::string;
#endif

}   // namespace klog

#endif  // KLOG_BASIC_TYPES_H_