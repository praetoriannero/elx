#ifndef ELX_ALLOC_H
#define ELX_ALLOC_H

#include <stdio.h>
#include <stdlib.h>

void* malloc_impl(size_t size, const char* file, int line);

void* realloc_impl(void* ptr, size_t size, const char* file, int line);

void free_impl(void* ptr, const char* file, int line);

void not_null_impl(void* ptr, const char* file, int line);

#define xmalloc(size) malloc_impl((size), __FILE__, __LINE__)

#define xrealloc(ptr, size) realloc_impl((ptr), (size), __FILE__, __LINE__)

#define xfree(ptr) free_impl((ptr), __FILE__, __LINE__)

#define xnotnull(ptr) not_null_impl((ptr), __FILE__, __LINE__)

#endif
