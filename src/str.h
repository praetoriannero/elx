#ifndef ELX_STR_H
#define ELX_STR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>


typedef struct string {
    char* data;
    size_t capacity;
    size_t size;
} string_t;

void string_init(string_t* self);

void string_deinit(string_t* self);

string_t* string_copy(string_t* self);

void string_push_char(string_t* str, char c);

void string_clear(string_t* self);

#endif

