#pragma once

#include "core/str.h"

typedef struct {
  const char* data;
  usize capacity;
  usize size;
} StringView;

StringView string_view_from_string(String* string);

bool string_view_equals(const StringView* lhs, const StringView* rhs);
