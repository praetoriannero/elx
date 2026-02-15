#include "token_tables.h"
#include "array.h"
#include "stdint.h"
#include "token_kind.h"
#include "xalloc.h"

lookup_result_t lookup_operator(char* chars) {
    xnotnull(chars);

    lookup_result_t result = {chars, TOK_INVALID};
    char* c_iter = chars;
    const op_node_t* op_iter = op_table;
    op_node_t op_node;
    size_t table_size = array_len(op_table);
    token_kind_t kind = TOK_INVALID;

    while (*c_iter) {
        if (!table_size && !op_iter) {
            panic("error detected in table structure, table_size=%zu, "
                  "table_ptr=%p",
                  table_size, op_iter);
        }

        for (uint32_t idx = 0; idx < table_size; idx++) {
            op_node = op_iter[idx];
            if (op_node.text == *c_iter) {
                kind = op_node.kind;
                op_iter = op_node.children;
                table_size = op_node.child_count;
                break;
            }
        }

        if ((op_iter == NULL) || (kind == TOK_INVALID)) {
            break;
        }

        c_iter++;
    }

    return result;
}

// lookup_result_t lookup_keyword(char* chars);
