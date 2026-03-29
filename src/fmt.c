#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"
#include "xalloc.h"

char* fmt(Arena* arena, const char* fmt, ...) {
    xnotnull((void*)fmt);

    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    if (n < 0) {
        return NULL;
    }

    char* buf = arena_alloc(arena, (size_t)n + 1);

    va_start(ap, fmt);
    vsnprintf(buf, (size_t)n + 1, fmt, ap);
    va_end(ap);

    return buf;
}
