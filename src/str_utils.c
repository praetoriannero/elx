#include <string.h>

#include "str_utils.h"

char* strdup(arena_t* arena, const char* str) {
    if (!str)
        return NULL;

    size_t len = strlen(str) + 1;
    char* copy = arena_alloc(arena, len);
    if (copy) {
        memcpy(copy, str, len);
    }

    return copy;
}
