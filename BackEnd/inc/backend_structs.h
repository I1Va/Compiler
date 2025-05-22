#ifndef BACKEND_STRUCTS_H
#define BACKEND_STRUCTS_H

#include "string_funcs.h"
#include "stack_funcs.h"
#include "AST_structs.h"

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

struct asm_glob_space {
    str_storage_t ** str_storage = NULL;

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

enum cpu_stack_elem_types {
    CPU_STACK_VAR_VALUE,
    CPU_STACK_CONSTANT,
    CPU_STACK_RETURN_ADDR,
    CPU_STACK_BASE_POINTER,
};

struct cpu_stack_elem_t {
    const char *name;
    cpu_stack_elem_types stack_elem_type;
    data_types data_type;
    multi_val_t data_value;
};



enum symbol_type {
    NONE_TYPE_SYMBOL,

    EXTERN_FUNCTION_SYMBOL,
    EXTERN_VARIABLE_SYMBOL,
    FUNCTION_SYMBOL,
    VARIABLE_SYMBOL,
};

enum symbol_binding_type {
    NONE_BINDING,

    GLOBAL_OBJ_SYMBOL,
    LOCAL_OBJ_SYMBOL,
};

enum section_types {
    UNDEF_SECTION, // для undefined символов
    TEXT_SECTION,
    DATA_SECTION,
};

struct function_symbol_info {
    data_types return_data_type;
    size_t     args_cnt;
    size_t     args_summary_nmemb;
};

struct variable_symbol_info {
    data_types var_data_type;
};

const function_symbol_info POISON_FUNC_SYM_INFO = {NONE_DATA_TYPE, 0};
const variable_symbol_info POISON_VAR_SYM_INFO  = {NONE_DATA_TYPE};

struct symbol_t {
    char *sym_name;
    symbol_type sym_type;
    symbol_binding_type sym_bind;
    section_types sym_section;
    int64_t section_offset;

    function_symbol_info func_sym_info;
    variable_symbol_info var_sym_info;
};

const symbol_t POISON_SYMBOL = {NULL, NONE_TYPE_SYMBOL, NONE_BINDING, UNDEF_SECTION, -1, POISON_FUNC_SYM_INFO, POISON_VAR_SYM_INFO};
const size_t MAX_SYMBOL_NAME_SZ = 64;
const size_t SYMBOL_TABLE_MAX_SZ = 128;
const size_t MAX_TEXT_SECTION_SZ = 1ul << 15;
const size_t MAX_DATA_SECTION_SZ = 1ul << 15;

struct symbol_table_t {
    size_t table_sz = 0;
    symbol_t data[SYMBOL_TABLE_MAX_SZ] = {};
};

struct asm_payload_t {
    char text_section[MAX_TEXT_SECTION_SZ] = {};
    int text_section_offset = 0;

    char data_section[MAX_DATA_SECTION_SZ] = {};
    int data_section_offset = 0;

    symbol_table_t symbol_table = {};
};

#endif // BACKEND_STRUCTS_H