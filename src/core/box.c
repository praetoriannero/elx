#include "core/box.h"

void box_init(Box* box, Allocator* alloc, void* ptr) {
  box->ptr = alloc->alloc(alloc, sizeof(ptr));
  box->alloc = alloc;
}

void box_deinit(Box* box) {}

void box_free(Box* box);
