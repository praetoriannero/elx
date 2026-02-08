#include <stdio.h>
#include <stdlib.h>

void* malloc_impl(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr,
            "Out of memory allocating %zu bytes at %s:%d\n",
            size, file, line);
        exit(1);
    }
    return ptr;
}
