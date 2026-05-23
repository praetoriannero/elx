#include <stdarg.h>
#include <stdio.h>

#include "core/allocator.h"
#include "core/xalloc.h"

char* fmt(Allocator* allocator, const char* fmt, ...) {
    xnotnull((void*)fmt);

    va_list ap;

    va_start(ap, fmt);
    i32 n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    if (n < 0) {
        return NULL;
    }

    char* buf = allocator_alloc(allocator, (usize)n + 1);

    va_start(ap, fmt);
    vsnprintf(buf, (usize)n + 1, fmt, ap);
    va_end(ap);

    return buf;
}
