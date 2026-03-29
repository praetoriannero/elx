#pragma once

#include "parser.h"

void print_expr(expr_t* expr, u64 depth);
void print_enum(enum_t* enum_, u64 depth);
void print_import(import_t* import, u64 depth);
void print_module(module_t* module, u64 depth);
void print_func(func_t* func, u64 depth);
void print_global(global_t* global, u64 depth);
void print_struct(struct_t* struct_, u64 depth);
void print_body(body_t* body, u64 depth);
