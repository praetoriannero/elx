#ifndef ELX_PANIC_H
#define ELX_PANIC_H

#include <stdarg.h>

#define panic(...) panic_impl(__FILE__, __LINE__, __func__, __VA_ARGS__)

void panic_impl(const char *file, int line, const char *func, const char *fmt,
                ...);

#endif
