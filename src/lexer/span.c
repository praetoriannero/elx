#include "lexer/span.h"
#include "core/fmt.h"
#include "core/str.h"
#include "core/xalloc.h"
#include <stdio.h>
#include <string.h>

void span_print(Span* span) { printf("%.*s", (int)(span->hi - span->lo), &span->buffer[span->lo]); }

SpanContext span_get_context(Span* self, Allocator* alloc) {
  xnotnull(self);
  xnotnull(alloc);
  xnotnull((void*)self->buffer);

  char c = {};

  String line_str = {};
  string_init(&line_str, alloc);

  String underline_str = {};
  string_init(&underline_str, alloc);

  SpanContext context = {};
  context.line = 1;
  context.column = 1;
  usize loc = 0;

  while (loc < self->lo) {
    c = self->buffer[loc++];

    if (c == '\n') {
      context.line++;
      context.column = 0;
      string_clear(&line_str);
      string_clear(&underline_str);
    } else {
      string_push(&line_str, c);
      string_push(&underline_str, ' ');
      context.column++;
    }
  }

  usize remain = (usize)(self->hi - self->lo);
  for (usize idx = 0; idx < remain; idx++) {
    string_push(&underline_str, '~');
  }

  while (true) {
    c = self->buffer[loc++];
    if (c == '\n') {
      break;
    }
    string_push(&line_str, c);
  }

  context.line_str = line_str;
  context.underline_str = underline_str;
  context.token_context = fmt(alloc, "%.*s", (u32)(self->hi - self->lo), &self->buffer[self->lo]);
  context.alloc = alloc;
  return context;
}

bool span_str_equal(Span* self, char* cstr) {
  usize cstr_len = strlen(cstr);
  usize span_len = self->hi - self->lo;
  if (cstr_len != span_len) {
    return false;
  }

  for (usize i = 0; i < span_len; i++) {
    if (self->buffer[self->lo + i] != cstr[i]) {
      return false;
    }
  }

  return true;
}
