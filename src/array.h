#pragma once

#include "allocator.h"
#include "panic.h"

typedef struct {
  void* data;
  usize length;
  usize datum_size;
} Array;

typedef struct {
  Array* arr;
  usize idx;
} ArrayIter;

#define array_len(x) (sizeof(x) / sizeof((x)[0]))

#define array_get(arr, i)                                                      \
  ((i) < array_len(arr) ? (arr)[i]                                             \
                        : (panic("array index out of bounds"), (arr)[0]))

void array_init(Array* self, Allocator* alloc, usize datum_size, usize capacity);

void array_init_ce(Array* self, usize datum_size, usize capacity);

Array* array_new(Allocator* alloc, usize datum_size, usize capacity);
