/*  HIR lowering performs the following:
 *  - macro expansion
 *  - desugaring
 *  - type checking
 *  - type inference
 */
#pragma once

#include "core/hashtable.h"
#include "parser/parser.h"

typedef struct {
  Ast* ast;
  HashTable scope; // HashTable<char*, module>
} HirContext;

void hir_init(HirContext* self, Allocator* alloc, Ast* ast);

void hir_lower_ast(HirContext* self);

void hir_type_check(HirContext* self);

void hir_type_inference(HirContext* self);
