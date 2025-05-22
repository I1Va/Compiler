#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "backend_structs.h"
#include "general.h"
#include "string_funcs.h"
#include "backend_utils.h"
#include "sections_processing.h"


static char GLOBAL_BUFER[BUFSIZ] = {};


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
        dump_symbol_table_t(stderr, symbol_table);
        RAISE_TRANSLATOR_ERROR("sym '%s' redefenition", symbol.sym_name);
        return;
    }

    symbol_table->data[symbol_table->table_sz++] = symbol;
};

symbol_t get_global_variable_sym_from_name_table(symbol_table_t *symbol_table, char *sym_name) {
    assert(symbol_table);
    assert(sym_name);

    for (size_t i = 0; i < symbol_table->table_sz; i++) {
        symbol_t cur_sym = symbol_table->data[i];

        if (strncmp(sym_name, cur_sym.sym_name, MAX_SYMBOL_NAME_SZ) == 0
            && cur_sym.sym_type == VARIABLE_SYMBOL) return cur_sym;
    }

    return POISON_SYMBOL;
}

void dump_symbol_table_t(FILE *stream, symbol_table_t *symbol_table) {
    fprintf_title(stream, "SYMBOL_TABLE", '=', STR_F_BORDER_SZ);
    for (size_t i = 0; i < symbol_table->table_sz; i++) {
        printf("symbol_table[%lu] = %s\n", i, symbol_table->data[i].sym_name);
    }
    fprintf_border(stream, '=', STR_F_BORDER_SZ, true);
}

bool get_data_from_file(const char path[], char bufer[]) {
    assert(path);
    assert(bufer);

    int lib_file_nmemb = get_file_sz(path);
    if (lib_file_nmemb < 0) {
        debug("failed to get file `%s` sz", path)
        return false;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        debug("failed to open %s", path);
        return false;
    }

    fread(bufer, (size_t) lib_file_nmemb, sizeof(char), file);
    fclose(file);

    return true;
}

bool load_std_lib(const char lib_data_section_path[], const char lib_text_section_path[], asm_payload_t *lib_payload) {
    assert(lib_text_section_path);
    assert(lib_data_section_path);
    assert(lib_payload);

    char data_section[MAX_DATA_SECTION_SZ] = {};
    if (!get_data_from_file(lib_data_section_path, data_section)) {
        debug("failed to fill lib data_section")
        return false;
    }
    MAKE_RECORD_IN_DATA_SECTION(lib_payload, "%s", data_section)

    char text_section[MAX_TEXT_SECTION_SZ] = {};
    if (!get_data_from_file(lib_text_section_path, text_section)) {
        debug("failed to fill lib text_section")
        return false;
    }
    MAKE_RECORD_IN_TEXT_SECTION(lib_payload, "%s", text_section)

    return true;
}
