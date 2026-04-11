#include "str.h"
#include "allocator.h"
#include "modprim.h"
#include "panic.h"
#include "str_utils.h"
#include "xalloc.h"

static const u64 MAX_STR_ALLOC = 4096;
static const usize INITIAL_STR_ALLOC = 4;

void string_push_char(Allocator* allocator, String* self, char c) {
  xnotnull(self);

  usize new_size = self->size + 1;
  if (new_size == self->capacity) {
    usize new_alloc = self->capacity * 2;
    char* new_data_ptr = (char*)allocator_realloc(allocator, self->data, new_alloc);
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

void string_init(Allocator* allocator, String* self) {
  xnotnull(self);

  *self = (String){
      .data = allocator_alloc(allocator, INITIAL_STR_ALLOC),
      .capacity = INITIAL_STR_ALLOC,
      .size = 0,
  };

  self->data[0] = '\0';
}

String string_copy(Allocator* allocator, String* self) {
  xnotnull(self);

  String string = (String){
      .size = self->size,
      .capacity = self->capacity,
      .data = strdup2(allocator, self->data),
  };

  return string;
}

void string_move(String* src, String* dst) {
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

void string_deinit(String* self) {
  if (!self) {
    return;
  }

  xfree(self->data);
}

void string_free(String* self) {
  xfree(self->data);
  xfree(self);
}
