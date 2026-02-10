#ifndef ELX_ARRAY_H
#define ELX_ARRAY_H

#include "panic.h"

#define array_len(x) (sizeof(x) / sizeof((x)[0]))

#define array_get(arr, i)                                                      \
    ((i) < array_len(arr) ? (arr)[i]                                           \
                          : (panic("array index out of bounds"), (arr)[0]))

#endif
