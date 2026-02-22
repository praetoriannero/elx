#include <stdint.h>

#include "arena.h"
#include "xalloc.h"

arena_t* arena_new(void) {
    arena_t* arena = xmalloc(sizeof(arena_t));
    arena_init(arena);
    return arena;
}

void arena_free(arena_t* self) {
    arena_deinit(self);
    xfree(self);
}

scope_t* arena_new_scope(arena_t* self) { return self->node_end; }

void arena_free_scope(arena_t* self, scope_t* scope) {
    while (self->node_end != scope) {
        node_t* node = self->node_end;
        self->node_end = node->parent;
        xfree(node);
    }
}

void arena_init(arena_t* self) {
    *self = (arena_t){
        .node_end = NULL,
    };
}

void arena_deinit(arena_t* self) { arena_free_scope(self, NULL); }

void* arena_alloc(arena_t* self, size_t size) {
    node_t* node = xmalloc(sizeof(*node) + size);
    void* ptr = (void*)(node + 1);
    *node = (node_t){
        .inner = ptr,
        .parent = self->node_end,
    };

    self->node_end = node;

    return ptr;
}
