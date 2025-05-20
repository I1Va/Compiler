#ifndef SECTIONS_PROCESSING_H
#define SECTIONS_PROCESSING_H

#include "translator_general.h"
#include "AST_structs.h"

const size_t MAX_SYMBOL_NAME_SZ = 64;
const size_t SYMBOL_TABLE_MAX_SZ = 128;
const size_t MAX_TEXT_SECTION_SZ = 1ul << 13;
const size_t MAX_DATA_SECTION_SZ = 1ul << 13;


#define MAKE_RECORD_IN_TEXT_SECTION(asm_payload, str_, ...) {                                                   \
    asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset,  \
                                                    MAX_TEXT_SECTION_SZ, str_, ##__VA_ARGS__);}

#define MAKE_RECORD_IN_DATA_SECTION(asm_payload, str_, ...) {                                                   \
    asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset,  \
                                                    MAX_TEXT_SECTION_SZ, str_, ##__VA_ARGS__);}

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

struct symbol_table_t {
    size_t table_sz = 0;
    symbol_t data[SYMBOL_TABLE_MAX_SZ] = {};
};

struct asm_payload_t {
    char text_section[MAX_TEXT_SECTION_SZ] = {};
    size_t text_section_offset = 0;

    char data_section[MAX_DATA_SECTION_SZ] = {};
    size_t data_section_offset = 0;

    symbol_table_t symbol_table = {};
};

int get_symbol_idx_in_name_table(symbol_table_t *symbol_table, const symbol_t symbol);
void add_symbol_to_name_table(symbol_table_t *symbol_table, symbol_t symbol);
symbol_t get_global_variable_sym_from_name_table(symbol_table_t *symbol_table, char *sym_name);

void dump_symbol_table_t(FILE *stream, symbol_table_t *symbol_table);

#endif // SECTIONS_PROCESSING_H