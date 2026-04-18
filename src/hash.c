#include "hash.h"

u64 fnv1a(u8* data, usize data_length, u64 prime) {
  u64 hash = 14695981039346656037U;

  for (usize i = 0; i < data_length; i++) {
    hash ^= data[i];
    hash *= prime;
  }
  return hash;
}

