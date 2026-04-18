#include "array.h"


void array_init(Array* self, Allocator* alloc, usize datum_size, usize capacity) {
  self->data = allocator_alloc(alloc, datum_size * capacity);
  self->length = capacity;
}

// Array* array_constexpr(Array* self, void* data, usize datum_size, usize capacity) {
//
// }


Array* array_new(Allocator* alloc, usize datum_size, usize capacity) {
  Array* arr = allocator_alloc(alloc, sizeof(Array));
  array_init(arr, alloc, datum_size, capacity);

  return arr;
}
