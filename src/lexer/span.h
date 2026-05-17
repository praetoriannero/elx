#pragma once

#include "core/modprim.h"

typedef struct {
  char* path;
  usize start;
  usize end;
} Span;

char* span_cstr(Span* self);
