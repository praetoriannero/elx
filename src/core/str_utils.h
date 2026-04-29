#pragma once

#include <stdbool.h>

#include "core/allocator.h"

char* str_copy(Allocator* allocator, const char* str);

char* strmov(char** str);

bool str_equal(const char* lhs, const char* rhs);
