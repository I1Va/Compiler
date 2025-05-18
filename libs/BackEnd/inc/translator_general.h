#ifndef TRANSLATOR_GENERAL_H
#define TRANSLATOR_GENERAL_H

#include <stdio.h>

#include "stack_funcs.h"
#include "general.h"

#define CHECK_AST_NODE_TYPE(node, exp_type)                                             \
    if (node->data.type != exp_type) {                                                  \
        RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.type)      \
    }

#define RAISE_TRANSLATOR_ERROR(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();

const size_t MAX_TEXT_SECTION_SZ = 1ul << 13;
const size_t MAX_DATA_SECTION_SZ = 1ul << 13;
const size_t ASM_STACK_CELL_NMEMB = 8;

struct asm_payload_t {
    char text_section[MAX_TEXT_SECTION_SZ] = {};
    size_t text_section_offset = 0;

    char data_section[MAX_DATA_SECTION_SZ] = {};
    size_t data_section_offset = 0;
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

struct var_t {
    int deep;
    int name_id;
    char *name;
    int loc_addr;
};

const var_t POISON_VAR = {-1, -1, NULL};

void var_t_fprintf(FILE *stream, void *elem_ptr);
void dump_global_info(FILE *stream, asm_glob_space *gl_space);
void dump_var_stack(FILE *stream, stack_t *var_stack);

#endif // TRANSLATOR_GENERAL_H