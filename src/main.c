#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/allocator.h"
#include "analyzer/analyzer.h"
#include "core/array.h"
#include "lexer/lexer.h"
#include "core/modprim.h"
#include "core/panic.h"
#include "parser/parser.h"

typedef FILE File;

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

void close_file(File** file) { fclose(*file); }

#define defer(func) __attribute__((__cleanup__(func)))

i32 main(i32 argc, char* argv[]) {
  File* file_handle defer(close_file) = NULL;
  char* file_name = NULL;
  char* content = NULL;
  i64 file_size = 0;

  Allocator allocator;
  allocator_init(&allocator);

  if (argc == 2) {
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
  printf("%jd\n", file_size);

  content = allocator_alloc(&allocator, (usize)file_size + 1);
  usize bytes_read = fread(content, 1, (usize)file_size, file_handle);
  content[bytes_read] = '\0';

  printf("CONTENT START\n%s\nCONTENT END\n", content);

  Lexer lexer = {};
  lexer_init(&lexer, &allocator, content, file_name);

  Parser parser = {};
  parser_init(&parser, &allocator, &lexer);

  Ast ast = parser_parse(&parser);
  print_ast(&ast);

  AnalyzerContext ast_ctx = {};
  analyzer_visit_ast(&ast, &ast_ctx);

  // clean up
  allocator_deinit(&allocator);

  return EXIT_SUCCESS;
}
