#pragma once

#include "allocator.h"
#include "modprim.h"

typedef void (*VectorFreeItem)(void* value, ...);

typedef struct vector {
  void* data;
  usize item_size;
  usize capacity;
  usize size;
  Allocator* alloc;
  VectorFreeItem free_item_cb;
} Vector;

typedef struct {
  Vector* vector;
  usize idx;
} VectorIter;

void vector_init(Vector* self, Allocator* allocator, usize item_size,
                 usize initial_capacity, VectorFreeItem free_item_cb);

Vector* vector_new(Allocator* allocator, usize item_size,
                   usize initial_capacity, VectorFreeItem free_item_cb);

void vector_push(Vector* self, const void* item);

void* vector_pop(Vector* self);

void* vector_get(Vector* self, usize index);

void vector_free(Vector* self);

void vector_deinit(Vector* self);

void vector_clear(Vector* self);

void vector_iter_init(VectorIter* self, Vector* vector);

bool vector_iter_next(VectorIter* self, void** element);

void vector_zero_fill(Vector* self);

#define vector_foreach(item, vec)                                              \
  VectorIter iter;                                                             \
  vector_iter_init(&iter, &vec);                                               \
  while (vector_iter_next(&iter, (void*)&item))

#define vector_iter(idx, vec) (usize idx = 0; idx < vec.size; idx++)
