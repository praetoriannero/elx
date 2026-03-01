#ifndef ELX_VECTOR_H
#define ELX_VECTOR_H

#include <stdlib.h>

#include "arena.h"
#include "modprim.h"

typedef struct vector {
    void* data;
    size_t datum_size;
    size_t capacity;
    size_t size;
} vector_t;

void vector_init(arena_t* arena, vector_t* self, size_t datum_size, size_t initial_capacity);

vector_t* vector_new(arena_t* arena, size_t datum_size, size_t initial_capacity);

void vector_push(arena_t* arena, vector_t* self, const void* datum);

void* vector_pop(vector_t* self);

void* vector_get(vector_t* self, usize index);

typedef void (*free_inner)(void* inner);

void vector_free(vector_t* self, free_inner inner_cb);

void vector_deinit(vector_t* self, free_inner inner_cb);

#endif
