#include "todo.h"
#include "panic.h"

void todo_impl(const char* file, const int line, const char* func) {
    panic("%s:%s at line %d not implemented\n", file, func, line);
}
