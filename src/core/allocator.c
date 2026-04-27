#include <stdint.h>
#include <string.h>

#include "core/allocator.h"
#include "core/panic.h"
#include "core/xalloc.h"

Allocator* allocator_new(void) {
  Allocator* allocator = xmalloc(sizeof(Allocator));
  allocator_init(allocator);
  return allocator;
}

#include <assert.h>

void allocator_free(Allocator* self, void* ptr) {
  AllocatorNode* node = ((AllocatorNode*)ptr) - 1;

  if (node->parent) {
    node->parent->child = node->child;
  }

  if (node->child) {
    node->child->parent = node->parent;
  }

  if (node == self->node_end) {
    self->node_end = node->parent;
  }

  xfree(node);
}

AllocatorScope* allocator_new_scope(Allocator* self) { return self->node_end; }

void allocator_free_scope(Allocator* self, AllocatorScope* scope) {
  while (self->node_end != scope) {
    AllocatorNode* node = self->node_end;
    self->node_end = node->parent;
    xfree(node);
  }
}

void allocator_init(Allocator* self) {
  *self = (Allocator){
      .node_end = NULL,
  };
}

void allocator_deinit(Allocator* self) {
  if (self) {
    allocator_free_scope(self, NULL);
  }
}

void* allocator_alloc(Allocator* self, usize size) {
  xnotnull(self);

  AllocatorNode* node = xmalloc(sizeof(*node) + size);
  void* ptr = (void*)(node + 1);

  *node = (AllocatorNode){
      .parent = self->node_end,
      .child = NULL,
      .size = size,
  };

  if (self->node_end) {
    self->node_end->child = node;
  }

  self->node_end = node;

  return ptr;
}

void* allocator_realloc(Allocator* self, void* old_ptr, usize new_size) {
  xnotnull(old_ptr);
  xnotnull(self);

  AllocatorNode* old_node = ((AllocatorNode*)old_ptr) - 1;

  if (old_node->size > new_size) {
    panic("illegal arena reallocation attempted with smaller new size\n");
  }

  AllocatorNode* new_node = xmalloc(sizeof(*new_node) + new_size);
  void* new_ptr = (void*)(new_node + 1);

  *new_node = (AllocatorNode){
      .parent = old_node->parent,
      .child = old_node->child,
      .size = new_size,
  };

  if (old_node->parent) {
    old_node->parent->child = new_node;
  }

  if (old_node->child) {
    old_node->child->parent = new_node;
  }

  if (old_node == self->node_end) {
    self->node_end = new_node;
  }

  memcpy(new_ptr, old_ptr, old_node->size);

  xfree(old_node);
  return new_ptr;
}
