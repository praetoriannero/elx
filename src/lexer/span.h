#pragma once

#include "core/modprim.h"

typedef struct {
  const char* path;
  const char* buffer;
  usize lo;
  usize hi;
} Span;

void span_print(Span* span);
