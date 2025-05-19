#ifndef AST_TRANSLATOR_H
#define AST_TRANSLATOR_H

#include "stack_funcs.h"
#include "AST_proc.h"
#include "translator_general.h"
#include "sections_processing.h"


void translate_ast_to_asm_code(ast_tree_t *tree);
void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_semicolon(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_function_definition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
size_t translate_func_args_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_var_identifier(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);


#endif // AST_TRANSLATOR_H