#include "strview.h"

StringView string_view_from_string(String* string) {
  StringView stringview = {
      .data = string->data,
      .capacity = string->capacity,
      .size = string->size,
  };

  return stringview;
}

bool string_view_equals(const StringView* lhs, const StringView* rhs);
