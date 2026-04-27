#pragma once

#include "core/modprim.h"

u64 fnv1a(u8* data, usize data_length);

u64 hash_str(const char* ptr);
