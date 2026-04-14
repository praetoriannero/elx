#pragma once

#include <stdint.h>

#include "allocator.h"
#include "modprim.h"

typedef struct string {
  char* data;
  usize capacity;
  usize size;
  Allocator* alloc;
} String;

String* string_new(char* str);

String* string_from(char* str);

void string_free(String* self);

void string_init(Allocator* allocator, String* self);

void string_deinit(String* self);

String string_copy(Allocator* allocator, String* self);

void string_move(String* src, String* dst);

void string_push(String* str, char c);

void string_extend(String* lhs, String rhs);

bool string_equals(const String* lhs, const String* rhs);
