#include "strview.h"

StringView stringview_from_string(String* string) {
  StringView stringview = {
      .data = string->data,
      .length = string->size,
  };

  return stringview;
}

StringView stringview_from_cstr(char* cstr, usize start, usize length) {
  StringView stringview = {
      .data = cstr + start,
      .length = length,
  };

  return stringview;
}

bool stringview_equal(const StringView* lhs, const StringView* rhs) {
  if (lhs->length != rhs->length) {
    return false;
  }

  for (usize i = 0; i < lhs->length; i++) {
    if (rhs->data[i] != lhs->data[i]) {
      return false;
    }
  }

  return true;
}

void stringview_pop(StringView* self) {
  if (self->length > 0) {
    self->length--;
  }
}

void stringview_push(StringView* self) {
  self->length++;
}
