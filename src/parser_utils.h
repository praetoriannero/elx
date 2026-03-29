#pragma once

#include "parser.h"

void print_expr(Expr* expr, u64 depth);
void print_enum(Enum* enum_, u64 depth);
void print_import(Import* import, u64 depth);
void print_module(Module* module, u64 depth);
void print_func(Func* func, u64 depth);
void print_global(Global* global, u64 depth);
void print_struct(Struct* struct_, u64 depth);
void print_body(Body* body, u64 depth);
