#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <backtrace.h>

#include "core/modprim.h"
#include "core/panic.h"

typedef struct StackFrame {
  const char* filename;
  i32 lineno;
  const char* function;
} StackFrame;

static StackFrame stack_trace[256];
static usize stack_idx = 0;

char PANIC_MSG_BUFF[PANIC_MSG_SIZE];

static void error_callback(void* data, const char* msg, int errnum) {
  (void)data;
  fprintf(stderr, "libbacktrace error: %s (%d)\n", msg, errnum);
}

static int full_callback(void* data, uintptr_t pc, const char* filename, int lineno, const char* function) {
  (void)data;
  (void)pc;

  if (!filename) {
    return 0;
  }

  if (strstr(function, "__libc")) {
    return 0;
  }
  stack_trace[stack_idx++] = (StackFrame){.filename=filename, .lineno=lineno, .function=function};

  return 0;
}

void print_stacktrace(void) {
  struct backtrace_state* state;

  state = backtrace_create_state(NULL, // use current executable
                                 0,    // not thread-safe
                                 error_callback, NULL);

  fprintf(stderr, "\n\e[0;33m");
  fprintf(stderr, "Traceback (most recent call first):\n");

  backtrace_full(state,
                 1, // skip this function
                 full_callback, error_callback, NULL);
  for (usize idx = stack_idx; idx != 0; --idx) {
    const char* filename = stack_trace[idx - 1].filename;
    i32 lineno = stack_trace[idx - 1].lineno;
    const char* function = stack_trace[idx - 1].function;
    fprintf(stderr, "  %zu: %s\n    at %s:%d\n", idx - 1, function ? function : "??", filename ? filename : "??",
            lineno);
  }
  fprintf(stderr, "\e[0m\n");
}

void panic_impl(const char* file, int line, const char* func, const char* fmt, ...) {
  print_stacktrace();

  fprintf(stderr, "\033\e[0;31mPANIC at %s:%d (%s): ", file, line, func);
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\e[0m\n");

  exit(1);
}
