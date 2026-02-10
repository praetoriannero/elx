#ifndef ELX_VECTOR_H
#define ELX_VECTOR_H

#include <stdlib.h>

typedef struct vector {
    void* data;
    size_t datum_size;
    size_t capacity;
    size_t size;
} vector_t;

void vector_init(vector_t* self, size_t datum_size, size_t initial_capacity);

vector_t* vector_new(size_t datum_size, size_t initial_capacity);

void vector_push(vector_t* self, const void* datum);

void* vector_pop(vector_t* self);

typedef void (*free_inner)(void* inner);

void vector_free(vector_t* self, free_inner inner_cb);

#endif
