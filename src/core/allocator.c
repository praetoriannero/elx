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

static inline void allocator_free_node(AllocatorNode* node) {
  xfree(node->buffer);
  xfree(node);
}

static inline AllocatorNode* allocator_find_node(Allocator* self, void* ptr) {
  AllocatorNode* node = NULL;
  xnotnull(self);

  if (!ptr) {
    return node;
  }
  
  if (!self->node_end) {
    panic("Pointer address not owned by allocator; allocator empty\n");
  }

  node = self->node_end;

  if (!node) {
    panic("Attempt to read from allocated null node\n");
  }

  while (node->buffer != ptr) {
    node = node->parent;
  }

  if (!node) {
    panic("Pointer address not owned by allocator\n");
  }

  return node;
}

void allocator_free(Allocator* self, void* ptr) {
  AllocatorNode* node = allocator_find_node(self, ptr);
  if (!node) {
    return;
  }

  if (node->parent) {
    node->parent->child = node->child;
  }

  if (node->child) {
    node->child->parent = node->parent;
  }

  if (node == self->node_end) {
    self->node_end = node->parent;
  }

#ifdef ELX_DEBUG
  self->total_alloc -= node->size;
#endif
  allocator_free_node(node);
}

AllocatorScope* allocator_new_scope(Allocator* self) { return self->node_end; }

void allocator_free_scope(Allocator* self, AllocatorScope* scope) {
  while (self->node_end != scope) {
    AllocatorNode* node = self->node_end;
    self->node_end = node->parent;
    allocator_free_node(node);
  }
}

void allocator_init(Allocator* self) {
  *self = (Allocator){};
}

void allocator_deinit(Allocator* self) {
  if (self) {
    allocator_free_scope(self, NULL);
    self->total_alloc = 0;
  }
}

void* allocator_alloc(Allocator* self, usize size) {
  xnotnull(self);

  AllocatorNode* node = xmalloc(sizeof(AllocatorNode));
  void* ptr = xmalloc(size);
  memset(ptr, 0, size);

  *node = (AllocatorNode){
      .parent = self->node_end,
      .child = NULL,
      .size = size,
      .buffer = ptr,
  };

  if (self->node_end) {
    self->node_end->child = node;
  }

  self->node_end = node;

#ifdef ELX_DEBUG
  self->total_alloc += size;
#endif

  return ptr;
}

void* allocator_realloc(Allocator* self, void* old_ptr, usize new_size) {
  xnotnull(old_ptr);
  xnotnull(self);

  AllocatorNode* node = allocator_find_node(self, old_ptr);
  void* new_ptr = xmalloc(new_size);
  memcpy(new_ptr, old_ptr, node->size);
  xfree(old_ptr);

  node->buffer = new_ptr;
  node->size = new_size;

#ifdef ELX_DEBUG
  self->total_alloc -= node->size;
  self->total_alloc += new_size;
#endif

  return new_ptr;
}

void allocator_move(Allocator* src, Allocator* dst, void* ptr) {
  if (src == dst) {
    return;
  }

  AllocatorNode* src_node = allocator_find_node(src, ptr);
  AllocatorNode* dst_node = allocator_alloc(dst, src_node->size);
  memcpy(dst_node->buffer, src_node->buffer, src_node->size);
  allocator_free_node(src_node);

#ifdef ELX_DEBUG
  src->total_alloc -= src_node->size;
#endif
}
