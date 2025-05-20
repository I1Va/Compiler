#ifndef TRANSLATOR_GENERAL_H
#define TRANSLATOR_GENERAL_H

#include <stdio.h>

#include "stack_funcs.h"
#include "general.h"
#include "AST_proc.h"
#include "FrontEnd.h"

#define CHECK_AST_NODE_TYPE(node, exp_type)                                             \
    if (node->data.ast_node_type != exp_type) {                                                  \
        RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.ast_node_type)      \
    }

#define RAISE_TRANSLATOR_ERROR(str_, ...) {fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();}

enum data_types {
    NONE_DATA_TYPE,
    INT64_DATA_TYPE,
    DOUBLE_DATA_TYPE,
    STRING_DATA_TYPE,
    VOID_DATA_TYPE,
};

enum data_types_nmemb {
    NONE_TYPE_NMEMB = 0,
    INT64_NMEMB = 8,
    STRING_PTR_NMEMB = 8,
    DOUBLE_NMEMB = 16,
};

const size_t DEFAULT_MANGLING_SUFFIX_SZ = 7;
const size_t ASM_STACK_CELL_NMEMB = 8;

struct asm_glob_space {
    int cur_scope_deep  = 0;
    int while_counter   = 0;
    int if_counter      = 0;
    bool func_init      = false;
    bool void_func      = false;

    const char *cur_func_name = NULL;

    stack_t cond_stack  = {};
    stack_t var_stack   = {};
    stack_t cpu_stack   = {};
};

const int UNKNOWN_BASE_POINTER_OFFSET = 0;

enum var_type_t {
    VAR_NONE_TYPE,
    VAR_TYPE_FUNCTION_ARG,
    VAR_TYPE_LOCAL_VAR,
};

struct var_t {
    data_types var_data_type;
    data_types_nmemb var_data_nmemb;

    int name_id;
    char *name;

    var_type_t var_type;
    int base_pointer_offset;
    int deep;
};

const var_t POISON_VAR_T = {NONE_DATA_TYPE, NONE_TYPE_NMEMB, -1, NULL, VAR_NONE_TYPE, 0, -1};
const char FUNCTION_LEAVE_SUFFIX[] = "_leave_action_";

void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz);
data_types convert_lexer_token_data_type(lexer_token_t token_type);
data_types_nmemb get_data_type_nmemb(data_types data_type);
void var_t_fprintf(FILE *stream, void *elem_ptr);
int get_data_type_descr(char bufer[], const size_t buf_sz, const data_types type);
void dump_var_stack(FILE *stream, stack_t *var_stack);
int get_var_type_t_descr(char bufer[], const size_t buf_sz, const var_type_t type);


#endif // TRANSLATOR_GENERAL_H