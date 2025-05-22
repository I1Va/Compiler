#include <assert.h>

#include "backend_utils.h"
#include "stack_output.h"
#include <stdlib.h>

static char GLOBAL_BUFER[BUFSIZ] = {};


void var_t_fprintf(FILE *stream, void *elem_ptr) {
    var_t var = *(var_t *) elem_ptr;
    fprintf(stream, "LOC VAR INIT(rbp_offset[%d], name='%s', id=%d, deep=%d)",
        var.base_pointer_offset, var.name, var.name_id, var.deep);
}

void dump_var_stack(FILE *stream, stack_t *var_stack) {
    assert(stream);
    assert(var_stack);

    fprintf_title(stream, "VAR_STACK", '=', STR_F_BORDER_SZ);
    fprintf(stream, "var_stack: \n");
    STACK_DUMP(var_stack, stream, var_t_fprintf);
}

data_types convert_lexer_token_data_type(lexer_token_t token_type) {
    switch (token_type) {
        case TOKEN_STRING_KEYWORD: return STRING_DATA_TYPE;
        case TOKEN_INT64_KEYWORD : return INT64_DATA_TYPE;
        case TOKEN_DOUBLE_KEYWORD: return DOUBLE_DATA_TYPE;
        case TOKEN_VOID          : return VOID_DATA_TYPE;
        default: get_token_descr(GLOBAL_BUFER, BUFSIZ, token_type); debug("wrong data_type token : {%s}\n", GLOBAL_BUFER); abort();
    }
}

int get_data_type_descr(char bufer[], const size_t buf_sz, const data_types type) {
    assert(bufer);
    #define T_DESCR_(type) case type: return snprintf(bufer, buf_sz, "%s", #type);
    switch (type) {
        T_DESCR_(NONE_DATA_TYPE)
        T_DESCR_(INT64_DATA_TYPE)
        T_DESCR_(DOUBLE_DATA_TYPE)
        T_DESCR_(STRING_DATA_TYPE)
        T_DESCR_(VOID_DATA_TYPE)
        default: return snprintf(bufer, buf_sz, "UNKNOWN_DATA_TYPE (%d)", type);
    }
    return 0;
}

int get_var_type_t_descr(char bufer[], const size_t buf_sz, const var_type_t type) {
    assert(bufer);

    #define T_DESCR_(type) case type: return snprintf(bufer, buf_sz, "%s", #type);
    switch (type) {
        T_DESCR_(VAR_NONE_TYPE)
        T_DESCR_(VAR_TYPE_FUNCTION_ARG)
        T_DESCR_(VAR_TYPE_LOCAL_VAR)
        default: return snprintf(bufer, buf_sz, "unknown var_type_t : (%d)", type);
    }
    #undef T_DESCR_

    return 0;
}

data_types_nmemb get_data_type_nmemb(data_types data_type) {
    switch (data_type) {
        case NONE_DATA_TYPE: return NONE_TYPE_NMEMB;
        case INT64_DATA_TYPE: return INT64_NMEMB;
        case DOUBLE_DATA_TYPE: return DOUBLE_NMEMB;
        case STRING_DATA_TYPE: return STRING_PTR_NMEMB;
        default: RAISE_TRANSLATOR_ERROR("get_data_type_nmemb failed (type %d)", data_type) return (data_types_nmemb) 0;
    }
}

void dump_global_info(FILE *stream, asm_glob_space *gl_space) {
    assert(stream);

    fprintf_title(stream, "GLOBAL_INFO", '=', STR_F_BORDER_SZ);
    fprintf(stream,
                   "cur_scope_deep: %d\n",
                   gl_space->cur_scope_deep);

    fprintf(stream, "var_stack: \n");
    STACK_DUMP(&gl_space->var_stack, stream, var_t_fprintf);
}

void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz) {
    assert(bufer);

    size_t prefix_sz = strnlen(prefix, buf_sz);
    snprintf(bufer, buf_sz, "%s", prefix);
    generate_random_string(bufer + prefix_sz, mangling_suf_sz);
}

int get_ast_node_descr(char bufer[], const size_t buf_sz, const ast_node_types node_type) {
    assert(bufer);

    #define T_DESCR_(node_type) case node_type: return snprintf(bufer, buf_sz, "`%s`", #node_type);
    switch (node_type) {
        T_DESCR_(AST_FUNC_INIT)
        T_DESCR_(AST_VAR_INIT)
        T_DESCR_(AST_VAR_ID)
        T_DESCR_(AST_FUNC_ID)
        T_DESCR_(AST_ASSIGN)
        T_DESCR_(AST_NUM_INT64)
        T_DESCR_(AST_NUM_DOUBLE)
        T_DESCR_(AST_STR_LIT)
        T_DESCR_(AST_RETURN)
        T_DESCR_(AST_BREAK)
        T_DESCR_(AST_CONTINUE)
        T_DESCR_(AST_CALL)
        T_DESCR_(AST_ELSE)
        T_DESCR_(AST_SCOPE)
        T_DESCR_(AST_WHILE)
        T_DESCR_(AST_SEMICOLON)
        T_DESCR_(AST_IF)
        T_DESCR_(AST_COMMA)
        T_DESCR_(AST_TYPE)
        T_DESCR_(AST_OPERATION)
        default: return snprintf(bufer, buf_sz, "UNKNOWN_NODE_TYPE(%d) ", node_type); break;
    }
    #undef T_DESCR_
}

int get_token_descr(char bufer[], const size_t buf_sz, const lexer_token_t token_type) {
    assert(bufer);

    #define T_DESCR_(lex) case lex: return snprintf(bufer, buf_sz, "`%s`", #lex);
    switch (token_type) {
        T_DESCR_(TOKEN_EOF)
        T_DESCR_(TOKEN_EMPTY)
        T_DESCR_(TOKEN_NUM_DOUBLE)
        T_DESCR_(TOKEN_BREAK)
        T_DESCR_(TOKEN_STRING_KEYWORD)
        T_DESCR_(TOKEN_CONTINUE)
        T_DESCR_(TOKEN_NUM_INT64)
        T_DESCR_(TOKEN_ADD)
        T_DESCR_(TOKEN_MUL)
        T_DESCR_(TOKEN_SUB)
        T_DESCR_(TOKEN_DIV)
        T_DESCR_(TOKEN_O_BRACE)
        T_DESCR_(TOKEN_C_BRACE)
        T_DESCR_(TOKEN_O_FIG_BRACE)
        T_DESCR_(TOKEN_C_FIG_BRACE)
        T_DESCR_(TOKEN_EOL)
        T_DESCR_(TOKEN_SPACE)
        T_DESCR_(TOKEN_POW)
        T_DESCR_(TOKEN_ID)
        T_DESCR_(TOKEN_IF)
        T_DESCR_(TOKEN_WHILE)
        T_DESCR_(TOKEN_SEMICOLON)
        T_DESCR_(TOKEN_LESS)
        T_DESCR_(TOKEN_MORE)
        T_DESCR_(TOKEN_LESS_EQ)
        T_DESCR_(TOKEN_MORE_EQ)
        T_DESCR_(TOKEN_EQ)
        T_DESCR_(TOKEN_INT64_KEYWORD)
        T_DESCR_(TOKEN_DOUBLE_KEYWORD)
        T_DESCR_(TOKEN_ASSIGN)
        T_DESCR_(TOKEN_COMMA)
        T_DESCR_(TOKEN_RETURN)
        T_DESCR_(TOKEN_ELSE)
        T_DESCR_(TOKEN_VOID)
        T_DESCR_(TOKEN_STR_LIT)
        default: return snprintf(bufer, buf_sz, "UNKNOWN_LEX(%d) ", token_type); break;
    }
    #undef T_DESCR_
}

int get_multi_val_t_descr(char bufer[], const size_t buf_sz, const multi_val_t val) {
    assert(bufer);

    return snprintf(bufer, buf_sz, "{%ld %lf %s}", val.int64_val, val.double_val, val.sval);
}


