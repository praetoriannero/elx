#include <string.h>

#include "allocator.h"
#include "str_utils.h"

char* elx_strdup(Allocator* allocator, const char* str) {
  if (!str)
    return NULL;

  size_t len = strlen(str) + 1;
  char* copy = allocator_alloc(allocator, len);
  memcpy(copy, str, len);

  return copy;
}

bool streq(const char* lhs, const char* rhs) { return (strcmp(lhs, rhs) == 0); }
