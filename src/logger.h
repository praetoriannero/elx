#pragma once

#include <stdarg.h>


#define log(fmt, ...) \
    do { \
        fprintf(stderr, "[%s:%d] " fmt, \
                __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
