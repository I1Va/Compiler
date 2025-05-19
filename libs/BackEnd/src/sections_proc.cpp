#include <assert.h>
#include <cstdlib>
#include <stdio.h>

#include "general.h"
#include "translator_general.h"
#include "sections_processing.h"


static char bufer[BUFSIZ] = {};

void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, data_types var_data_type, const multi_val_t var_value) {
    assert(asm_payload);
    assert(var_name);

    switch (var_data_type) {
        case INT64_DATA_TYPE    :  asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %ld\n", var_name, var_value.int64_val); return;
        case DOUBLE_DATA_TYPE   :  asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %lf\n", var_name, var_value.double_val); return;
        case STRING_DATA_TYPE   :  asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdb %s: '%s'\n", var_name, var_value.sval); return;
        case NONE_TYPE: RAISE_TRANSLATOR_ERROR("var_data_type: `NONE_TYPE`, expected: INT64_DATA_TYPE|DOUBLE_DATA_TYPE|STRING_PTR|STRING_LITERAL");
    }
}

void add_local_variable_to_asm_payload(asm_payload_t *asm_payload, char *var_name,  data_types var_data_type, const multi_val_t var_value) {
    assert(asm_payload);
    assert(var_name);

    switch (var_data_type) {
        case INT64_DATA_TYPE:
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "push   %ld", var_value.int64_val); return;

        case DOUBLE_DATA_TYPE:
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "sub    rsp, %d"
                "movdqu [rsp], %lf",
                DOUBLE_NMEMB, var_value.double_val); return;

        case STRING_DATA_TYPE: // Для указателя на строковый литерал создаем строковый литерал в секции .data
            generate_mangled_name(bufer, BUFSIZ, "str_literal_", DEFAULT_MANGLING_SUFFIX_SZ);
            add_global_variable_record_to_data_section(asm_payload, var_name, STRING_DATA_TYPE, var_value);
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "sub    rsp, %d"
                "lea    [rsp], %s",
                STRING_PTR_NMEMB, bufer); return;
        case NONE_TYPE:
            debug("error local variable type : NONE_TYPE!!\n"); abort();
    }
}

int get_symbol_idx_in_name_table(symbol_table_t *symbol_table, const symbol_t symbol) {
    assert(symbol_table);

    for (size_t i = 0; i < symbol_table->table_sz; i++) {
        symbol_t cur_symbol = symbol_table->data[i];
        if (strncmp(symbol.sym_name, cur_symbol.sym_name, MAX_SYMBOL_NAME_SZ) == 0) {
            return (int) i;
        }
    }

    return -1;
}

void add_symbol_to_name_table(symbol_table_t *symbol_table, symbol_t symbol) {
    if (get_symbol_idx_in_name_table(symbol_table, symbol) != -1) {
        RAISE_TRANSLATOR_ERROR("function '%s' redefenition", symbol.sym_name);
        return;
    }
};

