#ifndef SECTIONS_PROCESSING_H
#define SECTIONS_PROCESSING_H

#include "translator_general.h"
#include "AST_structs.h"

void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, data_types var_data_type, const multi_val_t var_value);
void add_local_variable_to_asm_payload(asm_payload_t *asm_payload, char *var_name, data_types var_data_type, const multi_val_t var_value);


#endif // SECTIONS_PROCESSING_H