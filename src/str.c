#include "str.h"
#include "arena.h"
#include "modprim.h"
#include "panic.h"
#include "str_utils.h"
#include "xalloc.h"

static const u64 MAX_STR_ALLOC = 4096;
static const usize INITIAL_STR_ALLOC = 4;

void string_push_char(arena_t* arena, string_t* self, char c) {
    xnotnull(self);

    usize new_size = self->size + 1;
    if (new_size == self->capacity) {
        usize new_alloc = self->capacity * 2;
        char* new_data_ptr = (char*)arena_realloc(arena, self->data, new_alloc);
        if (new_data_ptr == NULL) {
            panic("failed to reallocate string buffer\n");
        }
        if (new_alloc > MAX_STR_ALLOC) {
            panic("max string allocation exceeded\n");
        }

        self->data = new_data_ptr;
        self->capacity = new_alloc;
    }

    self->data[self->size] = c;
    self->data[self->size + 1] = '\0';
    self->size++;
}

void string_init(arena_t* arena, string_t* self) {
    xnotnull(self);

    *self = (string_t){
        .data = arena_alloc(arena, INITIAL_STR_ALLOC),
        .capacity = INITIAL_STR_ALLOC,
        .size = 0,
    };

    self->data[0] = '\0';
}

string_t string_copy(arena_t* arena, string_t* self) {
    xnotnull(self);

    string_t string = (string_t){
        .size = self->size,
        .capacity = self->capacity,
        .data = strdup(arena, self->data),
    };

    return string;
}

void string_move(string_t* src, string_t* dst) {
    xnotnull(src);
    xnotnull(dst);

    dst->data = src->data;
    dst->capacity = src->capacity;
    dst->size = src->size;

    src->data = NULL;
    src->capacity = 0;
    src->size = 0;
}

// void string_clear(string_t* self) {
//     xfree(self->data);
//     string_init(self);
// }

void string_deinit(string_t* self) {
    if (!self) {
        return;
    }

    xfree(self->data);
}

void string_free(string_t* self) {
    xfree(self->data);
    xfree(self);
}
