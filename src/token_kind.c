#include "token_kind.h"


char* token_kind_str(token_kind_t kind) {
    switch (kind) {
        case TOK_INVALID: return "TOK_INVALID";
        case TOK_QMARK: return "TOK_QMARK";
        case TOK_DQUOTE: return "TOK_DQUOTE";
        case TOK_SQUOTE: return "TOK_SQUOTE";
        case TOK_EXCLAM: return "TOK_EXCLAM";
        case TOK_COLON: return "TOK_COLON";
        case TOK_XOR: return "TOK_XOR";
        case TOK_PERCENT: return "TOK_PERCENT";
        case TOK_BINV: return "TOK_BINV";
        case TOK_MINUS: return "TOK_MINUS";
        case TOK_PLUS: return "TOK_PLUS";
        case TOK_DOLLAR: return "TOK_DOLLAR";
        case TOK_TICK: return "TOK_TICK";
        case TOK_STAR: return "TOK_STAR";
        case TOK_FSLASH: return "TOK_FSLASH";
        case TOK_BSLASH: return "TOK_BSLASH";
        case TOK_LBRACE: return "TOK_LBRACE";
        case TOK_RBRACE: return "TOK_RBRACE";
        case TOK_AT: return "TOK_AT";
        case TOK_EQ: return "TOK_EQ";
        case TOK_LPAREN: return "TOK_LPAREN";
        case TOK_RPAREN: return "TOK_RPAREN";
        case TOK_POUND: return "TOK_POUND";
        case TOK_AMPER: return "TOK_AMPER";
        case TOK_LBRACK: return "TOK_LBRACK";
        case TOK_RBRACK: return "TOK_RBRACK";
        case TOK_PIPE: return "TOK_PIPE";
        case TOK_OR: return "TOK_OR";
        case TOK_AND: return "TOK_AND";
        case TOK_SHL: return "TOK_SHL";
        case TOK_SHR: return "TOK_SHR";
        case TOK_GT: return "TOK_GT";
        case TOK_LT: return "TOK_LT";
        case TOK_GTE: return "TOK_GTE";
        case TOK_LTE: return "TOK_LTE";
        case TOK_EQEQ: return "TOK_EQEQ";
        case TOK_NOTEQ: return "TOK_NOTEQ";
        case TOK_PLUSEQ: return "TOK_PLUSEQ";
        case TOK_MINEQ: return "TOK_MINEQ";
        case TOK_TIMESEQ: return "TOK_TIMESEQ";
        case TOK_DIVEQ: return "TOK_DIVEQ";
        case TOK_BINVEQ: return "TOK_BINVEQ";
        case TOK_BANDEQ: return "TOK_BANDEQ";
        case TOK_BXOREQ: return "TOK_BXOREQ";
        case TOK_MODEQ: return "TOK_MODEQ";
        case TOK_SHLEQ: return "TOK_SHLEQ";
        case TOK_SHREQ: return "TOK_SHREQ";
        case TOK_KW_AS: return "TOK_KW_AS";
        case TOK_KW_ASYNC: return "TOK_KW_ASYNC";
        case TOK_KW_AWAIT: return "TOK_KW_AWAIT";
        case TOK_KW_BREAK: return "TOK_KW_BREAK";
        case TOK_KW_CONST: return "TOK_KW_CONST";
        case TOK_KW_CONTINUE: return "TOK_KW_CONTINUE";
        case TOK_KW_ELSE: return "TOK_KW_ELSE";
        case TOK_KW_ENUM: return "TOK_KW_ENUM";
        case TOK_KW_FALSE: return "TOK_KW_FALSE";
        case TOK_KW_FN: return "TOK_KW_FN";
        case TOK_KW_FOR: return "TOK_KW_FOR";
        case TOK_KW_IF: return "TOK_KW_IF";
        case TOK_KW_IMPL: return "TOK_KW_IMPL";
        case TOK_KW_IN: return "TOK_KW_IN";
        case TOK_KW_LET: return "TOK_KW_LET";
        case TOK_KW_MATCH: return "TOK_KW_MATCH";
        case TOK_KW_MODULE: return "TOK_KW_MODULE";
        case TOK_KW_MUT: return "TOK_KW_MUT";
        case TOK_KW_PUB: return "TOK_KW_PUB";
        case TOK_KW_REF: return "TOK_KW_REF";
        case TOK_KW_RETURN: return "TOK_KW_RETURN";
        case TOK_KW_SELF: return "TOK_KW_SELF";
        case TOK_KW_SELFTYPE: return "TOK_KW_SELFTYPE";
        case TOK_KW_STATIC: return "TOK_KW_STATIC";
        case TOK_KW_STRUCT: return "TOK_KW_STRUCT";
        case TOK_KW_SUPER: return "TOK_KW_SUPER";
        case TOK_KW_TRAIT: return "TOK_KW_TRAIT";
        case TOK_KW_TRUE: return "TOK_KW_TRUE";
        case TOK_KW_TYPE: return "TOK_KW_TYPE";
        case TOK_KW_USE: return "TOK_KW_USE";
        case TOK_KW_WHILE: return "TOK_KW_WHILE";
        case TOK_SEMICOLON: return "TOK_SEMICOLON";
        case TOK_IDENT: return "TOK_IDENT";
        case TOK_EOF: return "TOK_EOF";
        case TOK_ARROW: return "TOK_ARROW";
    }

    return "TOK_INVALID";
}

