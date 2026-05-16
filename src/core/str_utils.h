#pragma once

#include <stdbool.h>

#include "core/allocator.h"

char* str_copy(Allocator* allocator, const char* cstr);

char* str_move(char** cstr);

bool str_equal(const char* lhs, const char* rhs);

u64 str_hash(const char* cstr);
