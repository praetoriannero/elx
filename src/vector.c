#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "panic.h"
#include "xalloc.h"

#define DEFAULT_VEC_SIZE 8

void vector_init(arena_t* arena, vector_t* self, size_t datum_size, size_t initial_capacity) {
    xnotnull(self);

    self->data = arena_alloc(arena, datum_size * initial_capacity);
    self->capacity = initial_capacity;
    self->datum_size = datum_size;
    self->size = 0;
}

vector_t* vector_new(arena_t* arena, size_t datum_size, size_t initial_capacity) {
    vector_t* vec = arena_alloc(arena, sizeof(vector_t));
    vector_init(arena, vec, datum_size, initial_capacity);
    return vec;
}

void vector_push(arena_t* arena, vector_t* self, const void* datum) {
    xnotnull(self);

    size_t new_len = self->size + 1;
    if (new_len == self->capacity) {
        size_t new_capacity =
            self->capacity ? self->capacity * 2 : DEFAULT_VEC_SIZE;
        size_t new_alloc = new_capacity * self->datum_size;

        void* new_data = arena_realloc(arena, self->data, new_alloc);
        self->capacity = new_capacity;
        self->data = new_data;
    }

    memcpy((char*)self->data + self->size * self->datum_size, datum,
           self->datum_size);

    self->size++;
}

void* vector_pop(vector_t* self) {
    if (self->size == 0)
        return NULL;

    self->size--;

    return (u8*)self->data + self->size * self->datum_size;
}

void* vector_get(vector_t* self, usize index) {
    if (index > self->size) {
        panic("vector_get on index value greater than vector size");
    }

    return (u8*)self->data + (index * self->datum_size);
}

void vector_clear(vector_t* self, free_inner inner_cb) {
    size_t idx;
    if (inner_cb) {
        for (idx = 0; idx < self->size; idx++) {
            inner_cb((char*)self->data + idx * self->datum_size);
        }
    }

    self->size = 0;
    self->capacity = 0;
}

void vector_deinit(vector_t* self, free_inner inner_cb) {
    vector_clear(self, inner_cb);
}

void vector_free(vector_t* self, free_inner inner_cb) {
    size_t idx;
    if (inner_cb) {
        for (idx = 0; idx < self->size; idx++) {
            inner_cb((char*)self->data + idx * self->datum_size);
        }
    }

    xfree(self->data);
    xfree(self);
}
