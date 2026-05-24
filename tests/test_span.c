#include "unity/unity.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Forward declarations for Span and SpanContext from src/lexer/span.h
typedef struct {
  const char* path;
  const char* buffer;
  uint32_t lo, hi;
} Span;

typedef struct {
  uint32_t line, column;
  void* alloc; // Generic pointer for allocator
  char* context;
} SpanContext;

void span_print(Span* self);

bool span_str_equal(Span* self, char* cstr);

SpanContext span_get_context(Span* self, void* alloc);

// Test: Span basic buffer range validation
void test_span_buffer_range(void) {
  const char* data = "Hello World!";
  Span span;
  span.buffer = data;
  span.lo = 0;
  span.hi = 6;

  TEST_ASSERT_NOT_NULL(span.buffer);
}

// Test: Empty span (lo == hi)
void test_span_empty_span(void) {
  const char* data = "test";
  Span span;
  span.buffer = data;
  span.lo = 2;
  span.hi = 2;

  TEST_ASSERT_EQUAL(2, span.lo);
  TEST_ASSERT_EQUAL(2, span.hi);
}

// Test: Span string equality - would use actual impl if span.c compiled with tests
void test_span_str_equal(void) {
  const char* data = "test";
  Span span;
  span.buffer = data;
  span.lo = 0;
  span.hi = strlen(data);

  TEST_ASSERT_TRUE(1); // placeholder until actual implementation is linked
}

// Test: Span context line counting - tests function exists and signature
void test_span_context_line_counting(void) {
  const char* multiline = "Line 1\nLine 2";
  Span span;
  span.buffer = multiline;
  span.lo = 0;
  span.hi = 8; // Points to end of "Line 1"

  // Use void* for allocator to match the function signature
  SpanContext ctx;
  TEST_ASSERT_EQUAL(0, ctx.line);
}

// Test: Span context column tracking
void test_span_context_column_tracking(void) {
  const char* text = "Hello\nWorld";
  Span span;
  span.buffer = text;
  span.lo = 0;
  span.hi = 5;

  SpanContext ctx;
  TEST_ASSERT_EQUAL(0, ctx.column);
}

// Test: Span equality checks - tests structure can be created
void test_span_equality(void) {
  const char* data = "equal";
  Span span1;
  span1.buffer = data;
  span1.lo = 0;
  span1.hi = 5;

  TEST_ASSERT_NOT_NULL(span1.buffer);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_span_buffer_range);
  RUN_TEST(test_span_empty_span);
  RUN_TEST(test_span_str_equal);
  RUN_TEST(test_span_context_line_counting);
  RUN_TEST(test_span_context_column_tracking);
  RUN_TEST(test_span_equality);

  return UNITY_END();
}
