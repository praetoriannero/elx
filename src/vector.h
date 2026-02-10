#ifndef ELX_VECTOR_H
#define ELX_VECTOR_H

#include <stdlib.h>

typedef struct vector {
    void **data;
    size_t datum_size;
    size_t capacity;
    size_t size;
} vector_t;

vector_t *vector_new(size_t datum_size);

char *vector_push(vector_t *vec, void *datum);

void *vector_pop(vector_t *vec);

void vector_free(vector_t *vec);

#endif
