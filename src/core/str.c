#include <string.h>

#include "core/allocator.h"
#include "core/modprim.h"
#include "core/panic.h"
#include "core/str.h"
#include "core/str_utils.h"
#include "core/xalloc.h"

constexpr usize MAX_STR_ALLOC = 4096;
constexpr usize INITIAL_STR_ALLOC = 4;

String string_make(Allocator* alloc, char* cstr) {
  return string_from_cstr(cstr, alloc);
}

void string_push(String* self, char c) {
  xnotnull(self);

  usize new_size = self->size + 1;
  if (new_size == self->capacity) {
    usize new_alloc = self->capacity * 2;
    char* new_data_ptr = (char*)allocator_realloc(self->alloc, self->data, new_alloc);
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

void string_init(String* self, Allocator* allocator) {
  xnotnull(self);

  *self = (String){
      .data = allocator_alloc(allocator, INITIAL_STR_ALLOC),
      .capacity = INITIAL_STR_ALLOC,
      .size = 0,
      .alloc = allocator,
  };

  self->data[0] = '\0';
}

String string_copy(String* self, Allocator* allocator) {
  xnotnull(self);

  String string = (String){
      .size = self->size,
      .capacity = self->capacity,
      .data = str_copy(allocator, self->data),
      .alloc = self->alloc,
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

void string_deinit(String* self) {
  xnotnull(self);
  allocator_free(self->alloc, self->data);
}

String* string_new(Allocator* alloc) {
  String* string = allocator_alloc(alloc, sizeof(String));
  string_init(string, alloc);
  return string;
}

void string_free(String* self) {
  allocator_free(self->alloc, self->data);
  allocator_free(self->alloc, self);
}

String string_from_cstr(char* cstr, Allocator* alloc) {
  String string = (String){
      .size = strlen(cstr),
      .capacity = strlen(cstr) + 1,
      .data = str_copy(alloc, cstr),
      .alloc = alloc,
  };

  return string;
}

bool string_equal(const String* lhs, const String* rhs) {
  bool result = strcmp(lhs->data, rhs->data) == 0;
  return result;
}

void string_clear(String* self) {
  string_deinit(self);
  string_init(self, self->alloc);
}

u64 string_hash(String* self) { return str_hash(self->data); }
