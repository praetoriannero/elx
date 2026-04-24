#include "strview.h"

StringView stringview_from_string(String* string) {
  StringView stringview = {
      .data = string->data,
      .capacity = string->capacity,
      .size = string->size,
  };

  return stringview;
}

bool stringview_equals(const StringView* lhs, const StringView* rhs);
