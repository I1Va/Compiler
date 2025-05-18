#ifndef AST_TRANSLATOR_H
#define AST_TRANSLATOR_H

#include "stack_funcs.h"
#include "AST_proc.h"

const size_t MAX_TEXT_SECTION_SZ = 1ul << 13;
const size_t MAX_DATA_SECTIONS_SZ = 1ul << 13;
const size_t ASM_STACK_CELL_NMEMB = 8;

struct asm_payload_t {
    char text_section[MAX_TEXT_SECTION_SZ] = {};
    char data_section[MAX_DATA_SECTIONS_SZ] = {};
};

struct asm_glob_space {
    int cur_scope_deep = 0;
    int cur_frame_ptr  = 0;
    int while_counter  = 0;
    int if_counter     = 0;
    bool func_init     = false;
    bool void_func     = false;
    stack_t cond_stack = {};
    stack_t var_stack  = {};
};


void translate_ast_to_asm_code(ast_tree_t *tree);
void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_semicolon(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);
void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload);

#endif // AST_TRANSLATOR_H