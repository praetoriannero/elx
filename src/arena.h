#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node {
    struct node* parent;
    struct node* child;
    size_t size;
} node_t;

typedef node_t scope_t;

typedef struct arena {
    node_t* node_end;
} Arena;

Arena* arena_new(void);

void arena_free(Arena* arena, void* ptr);

scope_t* arena_new_scope(Arena* self);

void arena_free_scope(Arena* self, node_t* scope);

void* arena_alloc(Arena* self, size_t size);

void* arena_realloc(Arena* self, void* old_ptr, size_t new_size);

void arena_deinit(Arena* self);

void arena_init(Arena* self);
