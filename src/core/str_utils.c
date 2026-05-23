#include <string.h>

#include "core/allocator.h"
#include "core/hash.h"
#include "core/str_utils.h"

char* str_copy(Allocator* allocator, const char* cstr) {
    if (!cstr)
        return NULL;

    size_t len = strlen(cstr) + 1;
    char* cstr_copy = allocator_alloc(allocator, len);
    memcpy(cstr_copy, cstr, len);

    return cstr_copy;
}

u64 str_hash(const char* cstr) { return fnv1a((u8*)cstr, strlen(cstr)); }

bool str_equal(const char* lhs, const char* rhs) { return (strcmp(lhs, rhs) == 0); }
