#include <stdint.h>
#include <string.h>

#include "panic.h"
#include "str.h"
#include "str_utils.h"
#include "xalloc.h"

static const uint64_t MAX_STR_ALLOC = 4096;
static const size_t INITIAL_STR_ALLOC = 4;

void string_push_char(string_t* self, char c) {
    xnotnull(self);

    size_t new_size = self->size + 1;
    if (new_size == self->capacity) {
        size_t new_alloc = self->capacity * 2;
        char* new_data_ptr = (char*)xrealloc(self->data, new_alloc);
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

void string_init(string_t* self) {
    xnotnull(self);

    self->data = xmalloc(INITIAL_STR_ALLOC);
    self->data[0] = '\0';
    self->capacity = INITIAL_STR_ALLOC;
    self->size = 0;
}

string_t string_copy(string_t* self) {
    xnotnull(self);

    string_t string = (string_t){
        .size = self->size,
        .capacity = self->capacity,
        .data = strdup(self->data),
    };

    return string;
}

void string_clear(string_t* self) {
    xfree(self->data);
    string_init(self);
}

void string_deinit(string_t* self) {
    if (!self) {
        return;
    }

    xfree(self->data);
    string_init(self);
}

void string_free(string_t* self) {
    xfree(self->data);
    xfree(self);
}
