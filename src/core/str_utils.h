#pragma once

#include <stdbool.h>

#include "core/allocator.h"

char* elx_strdup(Allocator* allocator, const char* str);

char* strmov(char** str);

bool streq(const char* lhs, const char* rhs);
