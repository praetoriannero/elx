#include <stdarg.h>
#include <stdio.h>

#include "logger.h"

void log_impl(const char* file, int line, const char* fmt, ...) {
    fprintf(stderr, "[%s:%d] ", file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
