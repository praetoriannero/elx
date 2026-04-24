#pragma once

#include "core/allocator.h"
#include "core/panic.h"

typedef struct {
  void* data;
  usize length;
  usize item_size;
} Array;

typedef struct {
  Array* arr;
  usize idx;
} ArrayIter;

#define array_len(x) (sizeof(x) / sizeof((x)[0]))

// #define array_create(type, size, values)

void array_init(Array* self, Allocator* alloc, usize item_size, usize capacity);

void array_init_ce(Array* self, usize item_size, usize capacity);

static void* _array_get_impl(Array* self, const usize index) {
  if (index > self->length) {
    panic("vector_get on out of bounds index");
  }

  return (u8*)self->data + (index * self->item_size);
}

#define array_get(arr, type, index) *(type*)_array_get_impl(arr, index)

void array_insert(Array* self, const usize index, const void* item);

Array* array_new(Allocator* alloc, usize item_size, usize capacity);
