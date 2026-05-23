#include "lexer/span.h"
#include "core/fmt.h"
#include <stdio.h>
#include <string.h>

void span_print(Span* span) { printf("%.*s", (int)(span->hi - span->lo), &span->buffer[span->lo]); }

SpanContext span_get_context(Span* self, Allocator* alloc) {
    SpanContext context = {};
    usize loc = 0;
    while (loc < self->lo) {
        char c = self->buffer[loc++];

        if (c == '\n') {
            context.line++;
            context.column = 0;
        } else {
            context.column++;
        }
    }
    context.context = fmt(alloc, "%.*s", (u32)(self->hi - self->lo), self->buffer[self->lo]);
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
