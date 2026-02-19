#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct node {
    void* inner;
    struct node* parent;
} node_t;

typedef node_t scope_t;

typedef struct {
    node_t* node_end;
} arena_t;

arena_t* arena_new(void);

void arena_free(arena_t* self);

node_t* arena_new_scope(arena_t* self);

void arena_free_scope(arena_t* self, node_t* scope);

void* arena_alloc(arena_t* self, size_t size);

void arena_deinit(arena_t* self);

void arena_init(arena_t* self);
