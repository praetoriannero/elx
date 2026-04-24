#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/modprim.h"

typedef struct node {
  struct node* parent;
  struct node* child;
  usize size;
} AllocatorNode;

typedef AllocatorNode AllocatorScope;

typedef struct allocator {
  AllocatorNode* node_end;
} Allocator;

Allocator* allocator_new(void);

void allocator_free(Allocator* allocator, void* ptr);

AllocatorScope* allocator_new_scope(Allocator* self);

void allocator_free_scope(Allocator* self, AllocatorScope* scope);

void* allocator_alloc(Allocator* self, usize size);

void* allocator_realloc(Allocator* self, void* old_ptr, usize new_size);

void allocator_deinit(Allocator* self);

void allocator_init(Allocator* self);
