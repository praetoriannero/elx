#pragma once

#include "core/allocator.h"

typedef struct {
  void** ptr;
  Allocator* alloc;
} Box;

Box box_make(Allocator* alloc, void* ptr);

void box_init(Box* self, Allocator* alloc, void* ptr);

void box_deinit(Box* self);

Box* box_copy(Box* self, Allocator* alloc);

void box_free(Box* self);

void* box_unwrap(Box* self);

#define unwrap(box, type) (type*)box_unwrap(box)
