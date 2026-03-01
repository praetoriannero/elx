#pragma once

#include "arena.h"

char* strdup(arena_t* arena, const char* str);

char* strmov(char** str);
