#ifndef ELX_ALLOC_H
#define ELX_ALLOC_H

#include <stdio.h>
#include <stdlib.h>

void* malloc_impl(size_t size, const char* file, int line);

#define xmalloc(size) malloc_impl((size), __FILE__, __LINE__)

#endif

