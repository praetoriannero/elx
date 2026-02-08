#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "panic.h"

void panic_impl(
    const char* file,
    int line,
    const char* func,
    const char* fmt,
    ...
) {
    fprintf(stderr, "PANIC at %s:%d (%s): ", file, line, func);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

