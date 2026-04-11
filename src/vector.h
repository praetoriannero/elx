#ifndef ELX_VECTOR_H
#define ELX_VECTOR_H

#include "allocator.h"
#include "modprim.h"

typedef struct vector {
  void* data;
  usize datum_size;
  usize capacity;
  usize size;
} Vector;

void vector_init(Allocator* allocator, Vector* self, usize datum_size, usize initial_capacity);

Vector* vector_new(Allocator* allocator, usize datum_size, usize initial_capacity);

void vector_push(Allocator* allocator, Vector* self, const void* datum);

void* vector_pop(Vector* self);

void* vector_get(Vector* self, usize index);

typedef void (*VectorFreeInner)(void* inner);

void vector_free(Vector* self, VectorFreeInner inner_cb);

void vector_deinit(Vector* self, VectorFreeInner inner_cb);

void vector_clear(Vector* self, VectorFreeInner inner_cb);

#define vector_iter(iter, vec) (usize(iter) = 0; (iter) < (vec).size; iter++)

#endif
