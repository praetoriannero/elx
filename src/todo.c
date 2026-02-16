#include <stdio.h>
#include <stdlib.h>

#include "todo.h"


void todo_impl(const char* file, const int line, const char* func) {
    fprintf(stderr, "%s:%s at line %d not implemented\n", file, func, line);

    exit(1);
}
