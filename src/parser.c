#include "parser.h"


ast_t parser_parse(elx_parser_t* self) {}

elx_struct_t parser_visit_struct(elx_parser_t* self) {}

elx_module_t parser_visit_module(elx_parser_t* self) {}

elx_func_t parser_visit_func(elx_parser_t* self) {}

elx_global_t parser_visit_global(elx_parser_t* self) {}

elx_enum_t parser_visit_enum(elx_parser_t* self) {}

elx_import_t parser_visit_import(elx_parser_t* self) {}

elx_parser_t parser(lexer_t lexer) {
    return (elx_parser_t){
        .lexer = lexer,
        .parse = &parser_parse,
        .visit_struct = &parser_visit_struct,
        .visit_module = &parser_visit_module,
        .visit_func = &parser_visit_func,
        .visit_global = &parser_visit_global,
        .visit_enum = &parser_visit_enum,
        .visit_import = &parser_visit_import,
    };
}
