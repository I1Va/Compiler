#ifndef BACKEND_UTILS_H
#define BACKEND_UTILS_H

#include <string.h>
#include "stack_funcs.h"
#include "backend_structs.h"


#define TYPIZATION_ERROR_RAISE(type1, type2) \
    fprintf_red(stderr, "{%s} [%s: %d]: translator_typization_error: %\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();

#define CHECK_AST_NODE_TYPE(node, exp_type)                                                      \
    if (node->data.ast_node_type != exp_type) {                                                  \
        RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.ast_node_type)      \
    }

#define RAISE_TRANSLATOR_ERROR(str_, ...) {fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();}

const size_t DEFAULT_MANGLING_SUFFIX_SZ = 7;
const size_t ASM_STACK_CELL_NMEMB = 8;
const int UNKNOWN_BASE_POINTER_OFFSET = 0;

const char FUNCTION_LEAVE_SUFFIX[] = "_leave_action_";


int get_multi_val_t_descr(char bufer[], const size_t buf_sz, const multi_val_t val);
int get_token_descr(char bufer[], const size_t buf_sz, const lexer_token_t token_type);
int get_ast_node_descr(char bufer[], const size_t buf_sz, const ast_node_types node_type);
int get_data_type_descr(char bufer[], const size_t buf_sz, const data_types type);
int get_var_type_t_descr(char bufer[], const size_t buf_sz, const var_type_t type);
void var_t_fprintf(FILE *stream, void *elem_ptr);

void dump_var_stack(FILE *stream, stack_t *var_stack);

void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz);

data_types convert_lexer_token_data_type(lexer_token_t token_type);
data_types_nmemb get_data_type_nmemb(data_types data_type);


#endif // BACKEND_UTILS_H