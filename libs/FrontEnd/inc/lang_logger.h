#ifndef LANG_LOGGER_H
#define LANG_LOGGER_H

#include "FrontEnd.h"

int token_write(char bufer[], const size_t buf_sz, const lexer_token_t token_type);
int get_ast_node_descr(char bufer[], const size_t buf_sz, const ast_node_types node_type);
void lexem_dump(FILE *stream, parsing_block_t *data, lexem_t lexem);
void lexem_list_dump(FILE *stream, parsing_block_t *data);
void name_table_dump(FILE *stream, parsing_block_t *data);
void grule_dump(FILE *stream, enum grammar_rule_num grule);
void printf_multi_val_t(const char name[], const multi_val_t val);
void printf_token_value_t(const char name[], const token_value_t val);
int get_multi_val_t_descr(char bufer[], const size_t buf_sz, const multi_val_t val);

#endif // LANG_LOGGER_H