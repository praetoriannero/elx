#include <string.h>

#include "arena.h"
#include "str_utils.h"

char* strdup2(arena_t* arena, const char* str) {
    if (!str)
        return NULL;

    size_t len = strlen(str) + 1;
    char* copy = arena_alloc(arena, len);
    if (copy) {
        memcpy(copy, str, len);
    }

    return copy;
}

bool streq(const char* lhs, const char* rhs) { return (strcmp(lhs, rhs) == 0); }
