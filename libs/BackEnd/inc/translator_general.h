#ifndef TRANSLATOR_GENERAL_H
#define TRANSLATOR_GENERAL_H

#include <stdio.h>

#include "stack_funcs.h"
#include "general.h"
#include "AST_proc.h"

#define CHECK_AST_NODE_TYPE(node, exp_type)                                             \
    if (node->data.type != exp_type) {                                                  \
        RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.type)      \
    }

#define RAISE_TRANSLATOR_ERROR(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();

const size_t DEFAULT_MANGLING_SUFFIX_SZ = 7;
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

struct func_info_t {
    char *name;
    int return_type_num;
    size_t argc;
};

void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz);


#endif // TRANSLATOR_GENERAL_H