#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "panic.h"
#include "parser.h"
#include "str.h"
#include "token.h"
#include "xalloc.h"

int32_t OK = 0;
int32_t ERR = 1;

typedef struct {
    string_t token_str;
    size_t col_start;
    size_t col_end;
    size_t line_start;
    size_t line_end;
} Token;

typedef struct {
    Token* values;
} TokenArray;

int64_t get_file_size(FILE* handle) {
    if (fseek(handle, 0, SEEK_END) != 0) {
        return 0;
    };

    int64_t file_size = ftell(handle);
    if (file_size < 0) {
        return 0;
    }

    rewind(handle);
    return file_size;
}

char* read_file_content(const char* file_path);

int32_t main(int argc, char** argv) {
    FILE* file_handle = NULL;
    char* file_name;
    char* content;
    int64_t file_size;

    if (argc >= 2) {
        file_name = argv[1];
        file_handle = fopen(file_name, "r");
    } else {
        panic("missing source location argument\n");
    }

    if (file_handle != NULL) {
        printf("%s\n", file_name);
    } else {
        panic("failed to open file %s\n", file_name);
    }

    file_size = get_file_size(file_handle);
    if (file_size < 0) {
        panic("failed to read file %s\n", file_name);
    }
    printf("%d\n", (int)file_size);

    content = malloc((uint64_t)file_size + 1);
    size_t bytes_read = fread(content, 1, (uint64_t)file_size, file_handle);
    content[bytes_read] = '\0';

    printf("CONTENT START\n%s\nCONTENT END\n", content);

    lexer_t* lexer = xmalloc(sizeof(lexer_t));
    lexer_init(lexer, content);

    token_kind_t kind = TOK_INVALID;
    while (kind != TOK_EOF) {
        token_t* token = lexer_next(lexer);
        kind = token->kind;
        if (kind == TOK_COMMENT) {
            continue;
        }
        printf("%s\n", token_string(token));
        token_deinit(token);
    }

    // clean up
    fclose(file_handle);
    lexer_deinit(lexer);
    free(content);
    return OK;
}
