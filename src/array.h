#pragma once

#include "allocator.h"

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

#define array_create(T, S, V)

void array_init(Array* self, Allocator* alloc, usize item_size, usize capacity);

void array_init_ce(Array* self, usize item_size, usize capacity);

void* array_get(Array* self, const usize index);

void array_insert(Array* self, const usize index, const void* item);

Array* array_new(Allocator* alloc, usize item_size, usize capacity);
