#ifndef LANG_LOGGER_H
#define LANG_LOGGER_H

#include "FrontEnd.h"

int token_write(char bufer[], const size_t buf_sz, const lexer_token_t token_type);
void lexem_dump(FILE *stream, parsing_block_t *data, lexem_t lexem);
void lexem_list_dump(FILE *stream, parsing_block_t *data);
void name_table_dump(FILE *stream, parsing_block_t *data);
void grule_dump(FILE *stream, enum grammar_rule_num grule);

#endif // LANG_LOGGER_H