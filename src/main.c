#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "analyzer.h"
#include "allocator.h"
#include "lexer.h"
#include "modprim.h"
#include "panic.h"
#include "parser.h"

typedef FILE File;

#define null NULL

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

i32 main(i32 argc, char* argv[]) {
  File* file_handle = null;
  char* file_name = null;
  char* content = null;
  i64 file_size = 0;

  Allocator allocator;
  allocator_init(&allocator);

  if (argc >= 2) {
    file_name = argv[1];
    file_handle = fopen(file_name, "r");
  } else {
    panic("missing source location argument\n");
  }

  if (file_handle != null) {
    printf("%s\n", file_name);
  } else {
    panic("failed to open file %s\n", file_name);
  }

  file_size = get_file_size(file_handle);
  if (file_size < 0) {
    panic("failed to read file %s\n", file_name);
  }
  printf("%jd\n", file_size);

  content = allocator_alloc(&allocator, (usize)file_size + 1);
  usize bytes_read = fread(content, 1, (usize)file_size, file_handle);
  content[bytes_read] = '\0';

  printf("CONTENT START\n%s\nCONTENT END\n", content);

  Lexer* lexer = allocator_alloc(&allocator, sizeof(Lexer));
  lexer_init(lexer, content, file_name);

  Parser* parser = parser_new(&allocator, *lexer);
  Ast ast = parser_parse(&allocator, parser);
  print_ast(&ast);

  AnalyzerContext ast_ctx = {};
  analyzer_visit_ast(&ast, &ast_ctx);

  // clean up
  fclose(file_handle);
  allocator_deinit(&allocator);

  return EXIT_SUCCESS;
}
