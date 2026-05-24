#pragma once

#include "core/allocator.h"
#include "core/modprim.h"
#include "core/str.h"
#include <stdbool.h>

typedef struct {
  const char* path;
  const char* buffer;
  usize lo;
  usize hi;
} Span;

typedef struct {
  usize line;
  usize column;
  Allocator* alloc;
  char* token_context;
  String line_str;
  String underline_str;
} SpanContext;

void span_print(Span* self);

bool span_str_equal(Span* self, char* cstr);

SpanContext span_get_context(Span* self, Allocator* alloc);

#define span_ref_cstr(span_) (int)(span_->hi - span_->lo), &span_->buffer[span_->lo]

#define span_cstr(span_) (int)(span_.hi - span_.lo), &span_.buffer[span_.lo]
