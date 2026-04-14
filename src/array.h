#pragma once

// #include "allocator.h"
#include "panic.h"

// typedef struct {
//   void* data;
//   usize length;
// } Array;
//
// typedef struct {
//   Array* arr;
//   usize idx;
// } ArrayIter;

#define array_len(x) (sizeof(x) / sizeof((x)[0]))

#define array_get(arr, i) ((i) < array_len(arr) ? (arr)[i] : (panic("array index out of bounds"), (arr)[0]))

// static inline void array_init(Array* self, Allocator* alloc, usize datum_size, usize capacity) {
//   self->data = allocator_alloc(alloc, datum_size * capacity);
//   self->length = capacity;
// }
//
// static inline Array* array_new(Allocator* alloc, usize datum_size, usize capacity) {
//   Array* arr = allocator_alloc(alloc, sizeof(Array));
//   array_init(arr, alloc, datum_size, capacity);
//
//   return arr;
// }
