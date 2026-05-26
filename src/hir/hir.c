#include "hir/hir.h"
#include "core/str.h"
#include "core/box.h"
#include "core/hashtable.h"
#include "core/vector.h"
#include "parser/parser.h"

void hir_init(HirContext* self, Allocator* alloc, Ast* ast) {
  HirContext ctx = {};

  HashTable table = {};
  hash_table_init(&table, alloc, (HashFunc)string_hash, (KeyEqualFunc)string_equal, (CopyItem)string_copy,
                  (CopyItem)string_copy, (FreeItem)box_free, (FreeItem)box_copy);

  ctx.ast = ast;
  ctx.symbol_table = table;
  ctx.alloc = alloc;
}

void hir_lower_ast(HirContext* self) {
  Module* module = NULL;
  VectorIter iter = {};
  vector_iter_init(&iter, &self->ast->module_vec);
  while (vector_iter_next(&iter, (void**)&module)) {
    VectorIter node_iter = {};
    vector_iter_init(&node_iter, &module->ast_node_vec);
    AstNode* node = NULL;
    while (vector_iter_next(&node_iter, (void**)&node)) {
      String symbol_name = span_string(&node->span, self->alloc);
      Box symbol_box = box_make(self->alloc, node);
      hash_table_insert(&self->symbol_table, &symbol_name, &node); 
    }
  }
}
