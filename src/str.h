#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct string {
    char* data;
    size_t capacity;
    size_t size;
} string_t;

string_t* string_new(char* str);

string_t* string_from(char* str);

void string_free(string_t* self);

void string_init(string_t* self);

void string_deinit(string_t* self);

string_t string_copy(string_t* self);

void string_move(string_t* src, string_t* dst);

void string_push_char(string_t* str, char c);

void string_clear(string_t* self);
