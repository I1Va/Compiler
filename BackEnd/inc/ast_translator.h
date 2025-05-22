#ifndef AST_TRANSLATOR_H
#define AST_TRANSLATOR_H

#include "string_funcs.h"
#include "AST_structs.h"
#include "backend_structs.h"

const int FARGS_RBP_OFFSET = 16;
const bool FIRST_ARG_TRUE = true;


void translate_ast_to_asm_code(ast_tree_t *tree, str_storage_t **storage, const char outfile_path[], const char std_lib_dir[]);
void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_semicolon(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_function_definition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
size_t translate_func_args_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, bool first_arg_in_rev_order);
void translate_var_identifier(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_scope(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, data_types var_data_type, const multi_val_t var_value);
void add_local_variable_init_with_assignment_to_asm_payload(asm_payload_t *asm_payload, char *var_name, data_types var_data_type);
void translate_constant(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_func_call(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_return_node(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_func_call(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_operation(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_if(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_if_condition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, int cur_cond_counter);
void translate_while(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_while_condition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, int cur_counter);
void translate_assign(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);


#endif // AST_TRANSLATOR_H