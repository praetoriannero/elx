#pragma once

#include "core/allocator.h"

typedef struct {
  void** ptr;
  Allocator* alloc;
} Box;

void box_init(Box* box, Allocator* alloc, void* ptr);

void box_deinit(Box* box);

void box_free(Box* box);
