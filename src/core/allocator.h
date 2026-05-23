#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/modprim.h"

typedef struct __attribute__((aligned(sizeof(max_align_t)))) AllocatorNode {
    struct AllocatorNode* parent;
    struct AllocatorNode* child;
    usize size;
    void* buffer;
} AllocatorNode;

typedef AllocatorNode AllocatorScope;

typedef struct Allocator {
    AllocatorNode* node_end;
    usize total_alloc;
    void* (*alloc)(struct Allocator*, usize);
    void* (*realloc)(struct Allocator*, void*, usize);
    void (*free)(struct Allocator*, void*);
} Allocator;

Allocator* allocator_new(void);

void allocator_free(Allocator* allocator, void* ptr);

AllocatorScope* allocator_new_scope(Allocator* self);

void allocator_free_scope(Allocator* self, AllocatorScope* scope);

void* allocator_alloc(Allocator* self, usize size);

void* allocator_realloc(Allocator* self, void* old_ptr, usize new_size);

void allocator_deinit(Allocator* self);

void allocator_init(Allocator* self);

void allocator_move(Allocator* lhs, Allocator* rhs, void* ptr);

#define scoped_allocator(name)                                                                                         \
    __attribute__((__cleanup__(allocator_deinit))) Allocator name = {};                                                \
    allocator_init(&name);

void* new (Allocator* alloc, usize size);

void* resize(Allocator* alloc, void* old_ptr, usize new_size);

void delete (Allocator* alloc, void* ptr);

void move(Allocator* src, Allocator* dst, void* ptr);

void into(Allocator* alloc, void* ptr);

void* copy(Allocator* alloc, void* ptr, usize size);
