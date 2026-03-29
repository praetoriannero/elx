#pragma once

#include <stdbool.h>

#include "arena.h"

char* strdup2(Arena* arena, const char* str);

char* strmov(char** str);

bool streq(const char* lhs, const char* rhs);
