#pragma once
// IWYU pragma: keep

#include <stdarg.h>

#define log(...) log_impl(__FILE__, __LINE__, __VA_ARGS__)

void log_impl(const char* file, int line, const char* fmt, ...);
