#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "token_utils.h"

bool is_valid_ident(char* ident) {
    size_t size = strlen(ident);
    for (size_t i = 0; i < size; i++) {
        if ((i == 0) && (isdigit(ident[i]))) {
            return false;
        }
        if (!is_ident_char(ident[i])) {
            return false;
        }
    }
    return true;
}


bool is_ident_char(char c) {
    if (isalnum(c) || c == '_') {
        return true;
    }
    return false;
}

