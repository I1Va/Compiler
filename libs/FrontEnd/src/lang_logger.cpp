#include "AST_io.h"
#include "FrontEnd.h"
#include "lang_logger.h"
#include "general.h"
#include "string_funcs.h"

#include <assert.h>
#include <stdio.h>

int token_write(char bufer[], const size_t buf_sz, const lexer_token_t token_type) {
    assert(bufer);

    #define T_DESCR_(stream, lex) case lex: return snprintf(bufer, buf_sz, "`%s`", #lex);
    switch (token_type) {
        T_DESCR_(stream, TOKEN_EOF)
        T_DESCR_(stream, TOKEN_EMPTY)
        T_DESCR_(stream, TOKEN_NUM_DOUBLE)
        T_DESCR_(stream, TOKEN_NUM_INT64)
        T_DESCR_(stream, TOKEN_ADD)
        T_DESCR_(stream, TOKEN_MUL)
        T_DESCR_(stream, TOKEN_SUB)
        T_DESCR_(stream, TOKEN_DIV)
        T_DESCR_(stream, TOKEN_O_BRACE)
        T_DESCR_(stream, TOKEN_C_BRACE)
        T_DESCR_(stream, TOKEN_O_FIG_BRACE)
        T_DESCR_(stream, TOKEN_C_FIG_BRACE)
        T_DESCR_(stream, TOKEN_EOL)
        T_DESCR_(stream, TOKEN_SPACE)
        T_DESCR_(stream, TOKEN_POW)
        T_DESCR_(stream, TOKEN_ID)
        T_DESCR_(stream, TOKEN_IF)
        T_DESCR_(stream, TOKEN_WHILE)
        T_DESCR_(stream, TOKEN_SEMICOLON)
        T_DESCR_(stream, TOKEN_LESS)
        T_DESCR_(stream, TOKEN_MORE)
        T_DESCR_(stream, TOKEN_LESS_EQ)
        T_DESCR_(stream, TOKEN_MORE_EQ)
        T_DESCR_(stream, TOKEN_EQ)
        T_DESCR_(stream, TOKEN_INT64_KEYWORD)
        T_DESCR_(stream, TOKEN_DOUBLE_KEYWORD)
        T_DESCR_(stream, TOKEN_ASSIGN)
        T_DESCR_(stream, TOKEN_COMMA)
        T_DESCR_(stream, TOKEN_RETURN)
        T_DESCR_(stream, TOKEN_ELSE)
        T_DESCR_(stream, TOKEN_VOID)
        T_DESCR_(stream, TOKEN_STR_LIT)
        default: snprintf(bufer, buf_sz, "UNKNOWN_LEX(%d) ", token_type); break;
    }
    #undef T_DESCR_
}

void lexem_dump(FILE *stream, parsing_block_t *data, lexem_t lexem) {
    #define T_DESCR_(stream, lex, fmt, val) case lex: fprintf(stream, #lex"(" fmt ")", val); break;
    fprintf(stream, "[l:%3lu, s:%3lu]: ", lexem.text_pos.lines + 1, lexem.text_pos.syms + 1);
    switch (lexem.token_type) {
        T_DESCR_(stream, TOKEN_EOF, "%s", "")
        T_DESCR_(stream, TOKEN_EMPTY, "%s", "")
        T_DESCR_(stream, TOKEN_NUM_DOUBLE, "%lg", lexem.token_val.double_val)
        T_DESCR_(stream, TOKEN_NUM_INT64, "%ld", lexem.token_val.int64_val)
        T_DESCR_(stream, TOKEN_ADD, "%c", '+')
        T_DESCR_(stream, TOKEN_MUL, "%c", '*')
        T_DESCR_(stream, TOKEN_SUB, "%c", '-')
        T_DESCR_(stream, TOKEN_DIV, "%c", '/')
        T_DESCR_(stream, TOKEN_O_BRACE, "%c", '(')
        T_DESCR_(stream, TOKEN_C_BRACE, "%c", ')')
        T_DESCR_(stream, TOKEN_O_FIG_BRACE, "%c", '{')
        T_DESCR_(stream, TOKEN_C_FIG_BRACE, "%c", '}')
        T_DESCR_(stream, TOKEN_EOL, "%s", "\\n")
        T_DESCR_(stream, TOKEN_SPACE, "%c", ' ')
        T_DESCR_(stream, TOKEN_POW, "%s", "^")
        T_DESCR_(stream, TOKEN_ID, "%s", data->name_table[lexem.token_val.int64_val].name)
        T_DESCR_(stream, TOKEN_IF, "%s", data->keywords_table[lexem.token_val.int64_val].name)
        T_DESCR_(stream, TOKEN_WHILE, "%s", data->keywords_table[lexem.token_val.int64_val].name)
        T_DESCR_(stream, TOKEN_SEMICOLON, "%c", ';')
        T_DESCR_(stream, TOKEN_LESS, "%c", '<')
        T_DESCR_(stream, TOKEN_MORE, "%c", '>')
        T_DESCR_(stream, TOKEN_LESS_EQ, "%s", "<=")
        T_DESCR_(stream, TOKEN_MORE_EQ, "%s", ">=")
        T_DESCR_(stream, TOKEN_EQ, "%s", "==")
        T_DESCR_(stream, TOKEN_INT64_KEYWORD, "%s", "int64")
        T_DESCR_(stream, TOKEN_DOUBLE_KEYWORD, "%s", "float")
        T_DESCR_(stream, TOKEN_ASSIGN, "%s", "=")
        T_DESCR_(stream, TOKEN_COMMA, "%c", ',')
        T_DESCR_(stream, TOKEN_RETURN, "%s", "return")
        T_DESCR_(stream, TOKEN_ELSE, "%s", "else")
        T_DESCR_(stream, TOKEN_VOID, "%s", "void")
        T_DESCR_(stream, TOKEN_STR_LIT, "\"%s\"", lexem.token_val.string_val)
        default: fprintf(stream, "UNKNOWN_LEX(%d) ", lexem.token_type); break;
    }
    #undef T_DESCR_
}

void lexem_list_dump(FILE *stream, parsing_block_t *data) {
    fprintf_title(stream, "LEXEM_LIST_DUMP", '-', STR_F_BORDER_SZ);

    printf("len: [%lu]\n", data->lexem_list_size);
    for (size_t i = 0; i < data->lexem_list_size; i++) {
        lexem_t lexem = data->lexem_list[i];
        lexem_dump(stream, data, lexem);
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");

    fprintf_border(stream, '-', STR_F_BORDER_SZ, true);
}

void name_table_dump(FILE *stream, parsing_block_t *data) {
    fprintf(stream, "name_table[%p]\n{\n", data->name_table);

    for (size_t i = 0; i < data->name_table_sz; i++) {
        fprintf(stream, "    [%lu] = {'%s', [%d]}\n", i, data->name_table[i].name, data->name_table[i].token_type);
    }

    fprintf(stream, "}\n");
}

void grule_dump(FILE *stream, enum grammar_rule_num grule) {
    #define GR_DESCR_(stream, grule) case grule: fprintf(stream, #grule); break;

    switch (grule) {
        GR_DESCR_(stream, EMPTY_GRULE)
        GR_DESCR_(stream, GET_SYNTAX_ANALYSIS)
        GR_DESCR_(stream, GET_ADDITIVE_EXPRESSION)
        GR_DESCR_(stream, GET_MULTIPLICATIVE_EXPRESSION)
        GR_DESCR_(stream, GET_DIRECT_DECLARATOR)
        GR_DESCR_(stream, GET_ONE_ARG_FUNCTION_CALL)
        GR_DESCR_(stream, GET_PRIMARY_EXPRESSION)
        GR_DESCR_(stream, GET_CONSTANT)
        GR_DESCR_(stream, GET_VARIABLE)
        GR_DESCR_(stream, GET_SELECTION_STATEMENT)
        GR_DESCR_(stream, GET_STATEMENT)
        GR_DESCR_(stream, GET_ASSIGNMENT)
        GR_DESCR_(stream, GET_LOGICAL_EXPRESSION)
        GR_DESCR_(stream, GET_EXPRESSION)
        GR_DESCR_(stream, GET_WHILE)
        GR_DESCR_(stream, GET_VARIABLE_INITIALIZATION)
        GR_DESCR_(stream, GET_VARIABLE_INITIALIZATION_WITH_ASSIGNMENT)
        GR_DESCR_(stream, GET_FUNC_SEPARATED_INIT_ARGS)
        GR_DESCR_(stream, GET_FUNCTION_INITIALIZATION)
        GR_DESCR_(stream, GET_FUNC_IDENTIFICATOR)
        GR_DESCR_(stream, GET_STATEMENT_LIST)
        GR_DESCR_(stream, GET_GLOBAL_STATEMENT)
        GR_DESCR_(stream, GET_GRULE_DIVIDED_LIST)
        GR_DESCR_(stream, GET_FUNCTION_CALL)
        GR_DESCR_(stream, GET_SCOPE)
        GR_DESCR_(stream, GET_TYPE)

        default: fprintf(stream, "UNKNOWN_GRULE(%d) ", grule); break;
    }
    #undef GR_DESCR_
}
