#include <stdio.h>
#include <stdlib.h>

#include "panic.h"

void* malloc_impl(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr) {
        panic("out of memory allocating %zu bytes at %s:%d\n", size, file,
              line);
    }
    return ptr;
}

void* realloc_impl(void* ptr, size_t size, const char* file, int line) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        panic("out of memory reallocating %zu bytes at %s:%d\n", size, file,
              line);
    }

    return new_ptr;
}

void free_impl(void* ptr, const char* file, int line) {
    if (ptr == NULL) {
        panic("invalid free on NULL pointer at %s:%d\n", file, line);
    }

    free(ptr);
}

void not_null_impl(void* ptr, const char* file, int line) {
    if (ptr == NULL) {
        panic("attempt to dereference NULL pointer at %s:%d\n", file, line);
    }
}
