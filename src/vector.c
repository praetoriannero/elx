#include <stdlib.h>
#include "vector.h"


vector_t* vector_new(size_t datum_size) {
    vector_t* vec = (vector_t*)malloc(sizeof(vector_t));

    vec->data = NULL;
    vec->capacity = 1;
    vec->datum_size = datum_size;
    vec->size = 0;

    return vec;
}

char* vector_push(vector_t* vec, void* datum) {
    size_t new_len = vec->size + 1;
    if (new_len >= (vec->capacity / 2)) {
        size_t new_alloc = vec->capacity * 2;
        char* new_data_ptr = (char*)realloc(vec->data, new_alloc);
        if (new_data_ptr == NULL) {
            char* err_msg = "Error: failed to allocate vector buffer.\n";
            return err_msg;
        }
        vec->capacity = new_alloc;
    }

    vec->data[vec->size] = datum;
    vec->size++;

    return NULL;
}

void* vector_pop(vector_t* vec) {
    void* datum = NULL;

    if (vec->size) {
        datum = vec->data[vec->size];
        vec->size--;
        return datum;
    }

    return NULL;
}

void vector_free(vector_t* vec) {
    size_t idx;
    for (idx = 0; idx < vec->size; idx++) {
        free(vec->data[idx]);
    }

    free(vec);
}


