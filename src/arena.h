#pragma once

#include <stddef.h>
#include <stdint.h>

#include "modprim.h"

typedef struct node {
  struct node* parent;
  struct node* child;
  usize size;
} ArenaNode;

typedef ArenaNode ArenaScope;

typedef struct arena {
  ArenaNode* node_end;
} Arena;

Arena* arena_new(void);

void arena_free(Arena* arena, void* ptr);

ArenaScope* arena_new_scope(Arena* self);

void arena_free_scope(Arena* self, ArenaScope* scope);

void* arena_alloc(Arena* self, usize size);

void* arena_realloc(Arena* self, void* old_ptr, usize new_size);

void arena_deinit(Arena* self);

void arena_init(Arena* self);
