#pragma once

#include "str.h"

typedef struct {
  const char* data;
  usize capacity;
  usize size;
} StringView;

StringView stringview_from_string(String* string);

bool stringview_equals(const StringView* lhs, const StringView* rhs);
