#include "hash.h"
#include <string.h>

u64 fnv1a(u8* data, usize data_length) {
  u64 hash = 14695981039346656037ULL;

  for (usize i = 0; i < data_length; i++) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

u64 hash_str(const char* ptr) {
  return fnv1a((u8*)ptr, strlen(ptr));
}
