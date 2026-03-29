#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "lexer.h"
// #include "logger.h"
#include "modprim.h"
#include "panic.h"
#include "parser.h"
// #include "str.h"
#include "xalloc.h"

u8 OK = 0;
u8 ERR = 1;

i64 get_file_size(FILE* handle) {
    if (fseek(handle, 0, SEEK_END) != 0) {
        return 0;
    };

    i64 file_size = ftell(handle);
    if (file_size < 0) {
        return 0;
    }

    rewind(handle);
    return file_size;
}

char* read_file_content(const char* file_path);

i32 main(int argc, char** argv) {
    FILE* file_handle = NULL;
    char* file_name;
    char* content;
    i64 file_size;

    Arena arena;
    arena_init(&arena);

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
    printf("%d\n", (i32)file_size);

    content = xmalloc((u64)file_size + 1);
    usize bytes_read = fread(content, 1, (u64)file_size, file_handle);
    content[bytes_read] = '\0';

    printf("CONTENT START\n%s\nCONTENT END\n", content);

    Lexer* lexer = arena_alloc(&arena, sizeof(Lexer));
    lexer_init(lexer, content);

    Parser* parser = parser_new(&arena, *lexer);
    Ast ast = parser_parse(&arena, parser);
    print_ast(&ast);

    // clean up
    fclose(file_handle);
    // lexer_deinit(lexer);
    xfree(content);

    arena_deinit(&arena);
    return OK;
}
