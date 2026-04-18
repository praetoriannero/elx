#pragma once

#include "allocator.h"
#include "modprim.h"

typedef struct vector {
  void* data;
  usize datum_size;
  usize capacity;
  usize size;
  Allocator* alloc;
} Vector;

typedef struct {
  Vector* vector;
  usize idx;
} VectorIter;

void vector_init(Allocator* allocator, Vector* self, usize datum_size,
                 usize initial_capacity);

Vector* vector_new(Allocator* allocator, usize datum_size,
                   usize initial_capacity);

void vector_push(Vector* self, const void* datum);

void* vector_pop(Vector* self);

void* vector_get(Vector* self, usize index);

typedef void (*VectorFreeInner)(void* value, ...);

void vector_free(Vector* self, VectorFreeInner inner_cb);

void vector_deinit(Vector* self, VectorFreeInner inner_cb);

void vector_clear(Vector* self, VectorFreeInner inner_cb);

void vector_iter_init(VectorIter* self, Vector* vector);

bool vector_iter_next(VectorIter* self, void** element);

#define vector_foreach(item, vec)                       \
  VectorIter iter;                                      \
  vector_iter_init(&iter, &vec);                        \
  while (vector_iter_next(&iter, (void*)&item))         \
