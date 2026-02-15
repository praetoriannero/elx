#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "xalloc.h"

#define DEFAULT_VEC_SIZE 8

void vector_init(vector_t* self, size_t datum_size, size_t initial_capacity) {
    xnotnull(self);

    self->data = xmalloc(datum_size * initial_capacity);
    self->capacity = initial_capacity;
    self->datum_size = datum_size;
    self->size = 0;
}

vector_t* vector_new(size_t datum_size, size_t initial_capacity) {
    vector_t* vec = xmalloc(sizeof(vector_t));
    vector_init(vec, datum_size, initial_capacity);
    return vec;
}

void vector_push(vector_t* self, const void* datum) {
    size_t new_len = self->size + 1;
    if (new_len == self->capacity) {
        size_t new_capacity =
            self->capacity ? self->capacity * 2 : DEFAULT_VEC_SIZE;
        size_t new_alloc = new_capacity * self->datum_size;

        void* new_data = xrealloc(self->data, new_alloc);
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

    return (char*)self->data + self->size * self->datum_size;
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
