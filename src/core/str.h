#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "core/allocator.h"
#include "core/modprim.h"

typedef struct {
  char* data;
  usize capacity;
  usize size;
  Allocator* alloc;
} String;

String* string_new(Allocator* alloc);

String string_from_cstr(char* str, Allocator* alloc);

void string_free(String* self);

void string_init(String* self, Allocator* allocator);

void string_deinit(String* self);

String string_copy(String* self, Allocator* allocator);

void string_move(String* src, String* dst);

void string_push(String* str, char c);

void string_extend(String* lhs, String rhs);

bool string_equal(const String* lhs, const String* rhs);
