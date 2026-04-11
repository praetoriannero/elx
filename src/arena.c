#include <stdint.h>
#include <string.h>

#include "arena.h"
#include "panic.h"
#include "xalloc.h"

Arena* arena_new(void) {
  Arena* arena = xmalloc(sizeof(Arena));
  arena_init(arena);
  return arena;
}

void arena_free(Arena* self, void* ptr) {
  ArenaNode* node = ((ArenaNode*)ptr) - 1;

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

ArenaScope* arena_new_scope(Arena* self) { return self->node_end; }

void arena_free_scope(Arena* self, ArenaScope* scope) {
  while (self->node_end != scope) {
    ArenaNode* node = self->node_end;
    self->node_end = node->parent;
    xfree(node);
  }
}

void arena_init(Arena* self) {
  *self = (Arena){
      .node_end = NULL,
  };
}

void arena_deinit(Arena* self) {
  if (self) {
    arena_free_scope(self, NULL);
  }
}

void* arena_alloc(Arena* self, size_t size) {
  xnotnull(self);

  ArenaNode* node = xmalloc(sizeof(*node) + size);
  void* ptr = (void*)(node + 1);

  *node = (ArenaNode){
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

void* arena_realloc(Arena* self, void* old_ptr, size_t new_size) {
  xnotnull(old_ptr);
  xnotnull(self);

  ArenaNode* old_node = ((ArenaNode*)old_ptr) - 1;

  if (old_node->size > new_size) {
    panic("illegal arena reallocation attempted with smaller new size\n");
  }

  ArenaNode* new_node = xmalloc(sizeof(*new_node) + new_size);
  void* new_ptr = (void*)(new_node + 1);

  *new_node = (ArenaNode){
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
