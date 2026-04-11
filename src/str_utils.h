#pragma once

#include <stdbool.h>

#include "allocator.h"

char* strdup2(Allocator* allocator, const char* str);

char* strmov(char** str);

bool streq(const char* lhs, const char* rhs);
