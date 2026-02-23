#pragma once

#include <stdarg.h>

#define panic(...) panic_impl(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define PANIC_MSG_SIZE 256

static char PANIC_MSG_BUFF[PANIC_MSG_SIZE];

void panic_impl(const char* file, int line, const char* func, const char* fmt,
                ...);
