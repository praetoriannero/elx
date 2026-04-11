#pragma once

#include <stdint.h>

#include "arena.h"
#include "modprim.h"

typedef struct string {
  char* data;
  usize capacity;
  usize size;
} String;

String* string_new(char* str);

String* string_from(char* str);

void string_free(String* self);

void string_init(Arena* arena, String* self);

void string_deinit(String* self);

String string_copy(Arena* arena, String* self);

void string_move(String* src, String* dst);

void string_push_char(Arena* arena, String* str, char c);

// void string_clear(string_t* self);
