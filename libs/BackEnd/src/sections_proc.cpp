#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "general.h"
#include "string_funcs.h"
#include "translator_general.h"
#include "sections_processing.h"


static char bufer[BUFSIZ] = {};
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

symbol_t get_global_variable_sym_from_name_table(symbol_table_t *symbol_table, char *sym_name) {
    assert(symbol_table);
    assert(sym_name);

    for (size_t i = 0; i < symbol_table->table_sz; i++) {
        symbol_t cur_sym = symbol_table->data[i];

        if (strncmp(sym_name, cur_sym.sym_name, MAX_SYMBOL_NAME_SZ) == 0 && cur_sym.sym_type == VARIABLE_SYMBOL) return cur_sym;
    }

    return POISON_SYMBOL;
}
