#include <stdarg.h>
#include <stdio.h>

#include "core/logger.h"

void log_impl(const char* file, int line, const char* fmt, ...) {
  fprintf(stderr, "INFO [%s:%d] ", file, line);
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void debug_impl(const char* file, int line, const char* fmt, ...) {
#ifdef ELX_DEBUG
  fprintf(stderr, "DEBUG [%s:%d] ", file, line);
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
#endif
}
