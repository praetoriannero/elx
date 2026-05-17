#include <stdio.h>
#include "lexer/span.h"

void span_print(Span* span) {
  printf("%.*s", (int)(span->hi - span->lo), &span->buffer[span->lo]);
}
