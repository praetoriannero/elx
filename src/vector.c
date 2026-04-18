#include <stddef.h>
#include <string.h>

#include "allocator.h"
#include "panic.h"
#include "vector.h"
#include "xalloc.h"

#define DEFAULT_VEC_SIZE 8

void vector_init(Vector* self, Allocator* allocator, usize item_size,
                 usize initial_capacity, VectorFreeItem free_item_cb) {
  xnotnull(self);

  self->data = allocator_alloc(allocator, item_size * initial_capacity);
  self->capacity = initial_capacity;
  self->item_size = item_size;
  self->size = 0;
  self->alloc = allocator;
  self->free_item_cb = free_item_cb;
}

Vector* vector_new(Allocator* alloc, usize item_size,
                   usize initial_capacity, VectorFreeItem free_item_cb) {
  Vector* vec = allocator_alloc(alloc, sizeof(Vector));
  vector_init(vec, alloc, item_size, initial_capacity, free_item_cb);
  return vec;
}

void vector_push(Vector* self, const void* item) {
  xnotnull(self);

  usize new_len = self->size + 1;
  if (new_len > self->capacity) {
    usize new_capacity = self->capacity ? self->capacity * 2 : DEFAULT_VEC_SIZE;
    usize new_alloc = new_capacity * self->item_size;

    void* new_data = allocator_realloc(self->alloc, self->data, new_alloc);
    self->capacity = new_capacity;
    self->data = new_data;
  }

  memcpy((char*)self->data + self->size * self->item_size, item,
         self->item_size);

  self->size++;
}

void* vector_pop(Vector* self) {
  if (self->size == 0)
    return NULL;

  self->size--;

  return (u8*)self->data + self->size * self->item_size;
}

void* vector_get(Vector* self, usize index) {
  if (index > self->size) {
    panic("vector_get on out of bounds index");
  }

  return (u8*)self->data + (index * self->item_size);
}

static inline void _vector_free_items(Vector* self) {
  if (self->free_item_cb) {
    for (usize idx = 0; idx < self->size; idx++) {
      self->free_item_cb((u8*)self->data + idx * self->item_size);
    }
  }
}

void vector_clear(Vector* self) {
  _vector_free_items(self);
  self->size = 0;
  self->capacity = 0;
}

void vector_deinit(Vector* self) {
  vector_clear(self);
}

void vector_free(Vector* self) {
  _vector_free_items(self);
  allocator_free(self->alloc, self->data);
  allocator_free(self->alloc, self);
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

void vector_zero_fill(Vector* self) {
  memset(self->data, 0, self->item_size * self->capacity);
}
