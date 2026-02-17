#include <string.h>

#include "xalloc.h"
#include "str_utils.h"


char* strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* copy = xmalloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}
