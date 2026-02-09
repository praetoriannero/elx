#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "xalloc.h"


char* fmt(const char* fmt, ...) {
    xnotnull((void*)fmt);

    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    if (n < 0) {
        return NULL;
    }

    char *buf = xmalloc((size_t)n + 1);
    if (!buf) {
        return NULL;
    }

    va_start(ap, fmt);
    vsnprintf(buf, (size_t)n + 1, fmt, ap);
    va_end(ap);

    return buf;
}

