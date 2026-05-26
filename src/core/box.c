#include "core/box.h"
#include "core/allocator.h"
#include <string.h>

Box box_make(Allocator* alloc, void* ptr) {
  Box box = {};
  box_init(&box, alloc, ptr);
  return box;
}

void box_init(Box* self, Allocator* alloc, void* ptr) {
  self->ptr = alloc->alloc(alloc, sizeof(ptr));
  memcpy(&self->ptr, &ptr, sizeof(void*));
  self->alloc = alloc;
}

void box_deinit(Box* self) {
  self->alloc->free(self->alloc, self->ptr);
}

Box* box_copy(Box* self, Allocator* alloc) {
  Box* box = allocator_alloc(alloc, sizeof(Box));
  box_init(box, alloc, self->ptr);
  return box;
}

void box_free(Box* self) {
  box_deinit(self);
  self->alloc->free(self->alloc, self);
}

void* box_unwrap(Box* self) {
  return *self->ptr;
}
