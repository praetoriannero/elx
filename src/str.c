#include <stdint.h>

#include "str.h"
#include "panic.h"
#include "xalloc.h"

const static uint64_t MAX_STR_ALLOC = 4096;

void string_push_char(string_t* self, char c) {
    size_t new_len = self->size + 1;
    if (new_len >= (self->capacity / 2)) {
        size_t new_alloc = self->capacity * 2;
        char* new_data_ptr = (char*)realloc(self->data, new_alloc);
        if (new_data_ptr == NULL) {
            panic("Error: failed to reallocate string buffer.\n");
        }
        if (new_alloc > MAX_STR_ALLOC) {
            panic("Error: max string allocation exceeded.\n");
        }
        self->capacity = new_alloc;
    }

    self->data[self->size] = c;
    self->data[self->size + 1] = '\0';
    self->size++;
}

void string_init(string_t* self) {
    char* new_char = xmalloc(1);
    self->data = new_char;
    self->data[0] = '\0';
    self->capacity = 2;
    self->size = 0;
}

void string_clear(string_t* self) {
    free(self->data);
    char* new_char = xmalloc(1);
    self->data = new_char;
    self->data[0] = '\0';
    self->capacity = 2;
    self->size = 0;
}

void string_deinit(string_t* self) {
    free(self->data);
    free(self);
}

