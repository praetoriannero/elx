#pragma once

#include "core/str.h"

typedef struct {
  char* data;
  usize length;
} StringView;

StringView stringview_from_string(String* string);

StringView stringview_from_cstr(char* cstr, usize start, usize length);

bool stringview_equal(const StringView* lhs, const StringView* rhs);

void stringview_pop(StringView* self);

void stringview_push(StringView* self);
