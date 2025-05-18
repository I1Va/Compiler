#include <assert.h>
#include <stdio.h>

#include "translator_general.h"
#include "FrontEnd.h"
#include "lang_logger.h"


static char bufer[BUFSIZ] = {};

void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, int init_type, const multi_val_t var_value) {
    assert(asm_payload);
    assert(var_name);

    switch (init_type) {
        case TOKEN_INT64_KEYWORD : asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %ld\n", var_name, var_value.int64_val); return;
        case TOKEN_DOUBLE_KEYWORD: asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %lf\n", var_name, var_value.double_val); return;
        case TOKEN_STRING_KEYWORD: asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdb %s: '%s'\n", var_name, var_value.sval); return;
        default: token_write(bufer, BUFSIZ, (lexer_token_t) init_type); RAISE_TRANSLATOR_ERROR("invalid_token: {%s}, expected: TOKEN_STRING_KEYWORD|TOKEN_DOUBLE_KEYWORD|TOKEN_INT64_KEYWORD", bufer);
    }
}

void add_local_variable_to_asm_payload(asm_payload_t *asm_payload, char *var_name, int init_type, const multi_val_t var_value) {
    assert(asm_payload);
    assert(var_name);

    switch (init_type) {
        case TOKEN_INT64_KEYWORD:
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "push   %ld", var_value.int64_val); return;

        case TOKEN_DOUBLE_KEYWORD:
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "sub    rsp, 16"
                "movdqu [rsp], %lf",
                var_value.double_val); return;

        case TOKEN_STRING_KEYWORD:
            generate_mangled_name(bufer, BUFSIZ, "str_literal_", DEFAULT_MANGLING_SUFFIX_SZ);
            char *string_literal_global_name = bufer;
            add_global_variable_record_to_data_section(asm_payload, var_name, TOKEN_STRING_KEYWORD, var_value);
            asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                "sub    rsp, 8"
                "lea    [rsp], %s",
                string_literal_global_name); return;
    }
}
