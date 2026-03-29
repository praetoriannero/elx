#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "panic.h"
#include "vector.h"
#include "xalloc.h"

#define DEFAULT_VEC_SIZE 8

void vector_init(Arena* arena, Vector* self, usize datum_size, usize initial_capacity) {
    xnotnull(self);

    self->data = arena_alloc(arena, datum_size * initial_capacity);
    self->capacity = initial_capacity;
    self->datum_size = datum_size;
    self->size = 0;
}

Vector* vector_new(Arena* arena, usize datum_size, usize initial_capacity) {
    Vector* vec = arena_alloc(arena, sizeof(Vector));
    vector_init(arena, vec, datum_size, initial_capacity);
    return vec;
}

void vector_push(Arena* arena, Vector* self, const void* datum) {
    xnotnull(self);

    usize new_len = self->size + 1;
    if (new_len == self->capacity) {
        usize new_capacity = self->capacity ? self->capacity * 2 : DEFAULT_VEC_SIZE;
        usize new_alloc = new_capacity * self->datum_size;

        void* new_data = arena_realloc(arena, self->data, new_alloc);
        self->capacity = new_capacity;
        self->data = new_data;
    }

    memcpy((char*)self->data + self->size * self->datum_size, datum, self->datum_size);

    self->size++;
}

void* vector_pop(Vector* self) {
    if (self->size == 0)
        return NULL;

    self->size--;

    return (u8*)self->data + self->size * self->datum_size;
}

void* vector_get(Vector* self, usize index) {
    if (index > self->size) {
        panic("vector_get on out of bounds index");
    }

    return (u8*)self->data + (index * self->datum_size);
}

void vector_clear(Vector* self, VectorFreeInner inner_cb) {
    usize idx;
    if (inner_cb) {
        for (idx = 0; idx < self->size; idx++) {
            inner_cb((char*)self->data + idx * self->datum_size);
        }
    }

    self->size = 0;
    self->capacity = 0;
}

void vector_deinit(Vector* self, VectorFreeInner inner_cb) { vector_clear(self, inner_cb); }

void vector_free(Vector* self, VectorFreeInner inner_cb) {
    usize idx;
    if (inner_cb) {
        for (idx = 0; idx < self->size; idx++) {
            inner_cb((char*)self->data + idx * self->datum_size);
        }
    }

    xfree(self->data);
    xfree(self);
}
