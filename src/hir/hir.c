#include "hir/hir.h"
#include "core/hashtable.h"
#include "parser/parser.h"

AstNode* copy_ast_node(AstNode* node, Allocator* alloc) { return node; }

void free_ast_node(AstNode* node) {}

void hir_init(HirContext* self, Allocator* alloc, Ast* ast) {
  HirContext ctx = {};

  HashTable table = {};
  hash_table_init(&table, alloc, (HashFunc)string_hash, (KeyEqualFunc)string_equal, (CopyItem)string_copy,
                  (CopyItem)string_copy, (FreeItem)string_free, (FreeItem)string_free);
  ctx.ast = ast;
  ctx.scope = table;
}
