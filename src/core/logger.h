#pragma once

#include <stdarg.h>

void log_impl(const char* file, int line, const char* fmt, ...);

void debug_impl(const char* file, int line, const char* fmt, ...);

#define log(...) log_impl(__FILE__, __LINE__, __VA_ARGS__)

#define debug(...) debug_impl(__FILE__, __LINE__, __VA_ARGS__)
