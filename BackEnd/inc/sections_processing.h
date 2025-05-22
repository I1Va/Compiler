#ifndef SECTIONS_PROCESSING_H
#define SECTIONS_PROCESSING_H

#include "backend_structs.h"


#define MAKE_RECORD_IN_TEXT_SECTION(asm_payload, str_, ...) {                                                   \
    asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset,  \
                                                    MAX_TEXT_SECTION_SZ, str_, ##__VA_ARGS__);}

#define MAKE_RECORD_IN_DATA_SECTION(asm_payload, str_, ...) {                                                   \
    asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset,  \
                                                    MAX_DATA_SECTION_SZ, str_, ##__VA_ARGS__);}

int get_symbol_idx_in_name_table(symbol_table_t *symbol_table, const symbol_t symbol);
void add_symbol_to_name_table(symbol_table_t *symbol_table, symbol_t symbol);
symbol_t get_global_variable_sym_from_name_table(symbol_table_t *symbol_table, char *sym_name);

void dump_symbol_table_t(FILE *stream, symbol_table_t *symbol_table);

bool get_data_from_file(const char path[], char bufer[]);
bool load_std_lib(const char lib_data_section_path[], const char lib_text_section_path[], asm_payload_t *lib_payload);


#endif // SECTIONS_PROCESSING_H