#include <stddef.h>
#include <string.h>

#include "panic.h"
#include "vector.h"
#include "xalloc.h"

#define DEFAULT_VEC_SIZE 8

void vector_init(Allocator* allocator, Vector* self, usize datum_size,
                 usize initial_capacity) {
  xnotnull(self);

  self->data = allocator_alloc(allocator, datum_size * initial_capacity);
  self->capacity = initial_capacity;
  self->datum_size = datum_size;
  self->size = 0;
  self->alloc = allocator;
}

Vector* vector_new(Allocator* allocator, usize datum_size,
                   usize initial_capacity) {
  Vector* vec = allocator_alloc(allocator, sizeof(Vector));
  vector_init(allocator, vec, datum_size, initial_capacity);
  return vec;
}

void vector_push(Vector* self, const void* datum) {
  xnotnull(self);

  usize new_len = self->size + 1;
  if (new_len == self->capacity) {
    usize new_capacity = self->capacity ? self->capacity * 2 : DEFAULT_VEC_SIZE;
    usize new_alloc = new_capacity * self->datum_size;

    void* new_data = allocator_realloc(self->alloc, self->data, new_alloc);
    self->capacity = new_capacity;
    self->data = new_data;
  }

  memcpy((char*)self->data + self->size * self->datum_size, datum,
         self->datum_size);

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

void vector_deinit(Vector* self, VectorFreeInner inner_cb) {
  vector_clear(self, inner_cb);
}

void vector_free(Vector* self, VectorFreeInner inner_cb) {
  usize idx;
  if (inner_cb) {
    for (idx = 0; idx < self->size; idx++) {
      inner_cb((char*)self->data + idx * self->datum_size);
    }
  }

  allocator_free(self->alloc, self->data);
  xfree(self);
}

void vector_iter_init(VectorIter* self, Vector* vector) {
  self->vector = vector;
  self->idx = 0;
}

void vector_iter_reset(VectorIter* self) { self->idx = 0; }

bool vector_iter_next(VectorIter* self, void** element) {
  if (self->idx < self->vector->size) {
    *element = vector_get(self->vector, self->idx);
    self->idx++;
    return true;
  }

  return false;
}
