#include "array.h"


void array_init(Array* self, Allocator* alloc, usize item_size, usize capacity) {
  self->data = allocator_alloc(alloc, item_size * capacity);
  self->length = capacity;
}

Array* array_new(Allocator* alloc, usize item_size, usize capacity) {
  Array* arr = allocator_alloc(alloc, sizeof(Array));
  array_init(arr, alloc, item_size, capacity);

  return arr;
}
