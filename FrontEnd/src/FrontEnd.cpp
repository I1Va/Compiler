#include <assert.h>
#include <stddef.h>

#include "FrontEnd.h"
#include "AST_proc.h"
#include "AST_io.h"
#include "lang_lexer.h"
#include "lang_grammar.h"
#include "general.h"
#include "string_funcs.h"

bool keywords_table_fill(keyword_t keywords_table[], size_t *keywords_table_size) {
    keyword_t temp_table[] =
    {
        {"EMPTY_NAME", 10, TOKEN_EMPTY},
        {"if", 2, TOKEN_IF},
        {"while", 5, TOKEN_WHILE},
        {"int", 3, TOKEN_INT64_KEYWORD},
        {"double", 6, TOKEN_DOUBLE_KEYWORD},
        {"string", 6, TOKEN_STRING_KEYWORD},
        {"return", 6, TOKEN_RETURN},
        {"else", 4, TOKEN_ELSE},
        {"void", 4, TOKEN_VOID},
    };

    *keywords_table_size = sizeof(temp_table) / sizeof(keyword_t);
    if (*keywords_table_size > KEY_WORD_TABLE_MAX_SIZE) {
        return false;
    }

    for (size_t i = 0; i < *keywords_table_size; i++) {
        keywords_table[i] = temp_table[i];
    }

    return true;
}

bool parsing_block_t_ctor(parsing_block_t *data, char *text,
    keyword_t keywords_table[], name_t *name_table,
    lexem_t *lexem_list, str_storage_t **storage)
{
    assert(data);
    assert(text);
    assert(keywords_table);
    assert(name_table);
    assert(lexem_list);
    assert(storage);

    data->text_idx = 0;
    data->text = text;

    data->lexem_list = lexem_list;
    data->lexem_list_idx = 0;
    data->lexem_list_size = 0;

    data->parser_err = {};

    data->storage = storage;
    data->keywords_table = keywords_table;
    data->name_table = name_table;
    keywords_table_fill(data->keywords_table, &data->keywords_table_sz);

    data->name_table = name_table;
    data->name_table_sz = 0;

    return true;
}

void parsing_block_t_dtor(parsing_block_t *data) {
    if (data && data->asm_code_file_ptr) {
        fclose(data->asm_code_file_ptr);
    }
}

bool FrontEnd_make_AST(ast_tree_t *ast_tree, str_storage_t **storage, const char high_level_code_inpath[], const char ast_tree_outpath[], const char ast_img_outpath[]) {
    assert(ast_tree);
    assert(high_level_code_inpath);
    assert(storage);

    dot_code_t dot_code       = {}; dot_code_t_ctor(&dot_code, LIST_DOT_CODE_PARS);
    str_t text                = {};

    lexem_t lexem_list[LEXEM_LIST_MAX_SIZE]           = {};
    keyword_t keywords_table[KEY_WORD_TABLE_MAX_SIZE] = {};
    name_t name_table[NAME_TABLE_MAX_SIZE]            = {};
    parsing_block_t data = {};


    text = read_text_from_file(high_level_code_inpath);
    if (!parsing_block_t_ctor(&data, text.str_ptr, keywords_table, name_table, lexem_list, storage)) {
        debug("parsing_block_t_ctor failed\n");
        CLEAR_MEMORY(exit_mark);
    }

    lex_scanner(&data);

    ast_tree->root = get_syntax_analysis(&data);

    if (check_parser_err(stdout, &data)) {
        CLEAR_MEMORY(exit_mark);
    }

    if (convert_subtree_to_dot(ast_tree->root, &dot_code, storage) == -1) {
        debug("convert_subtree_to_dot failed\n");
        CLEAR_MEMORY(exit_mark);
    }

    if (!dot_code_create_ast_img(&dot_code, ast_img_outpath)) {
        debug("dot_code_dump_AST failed\n");
        CLEAR_MEMORY(exit_mark);
    }

    ast_tree_file_dump(ast_tree_outpath, ast_tree, AST_TREE_DUMP_INDENT);

    FREE(text.str_ptr);
    parsing_block_t_dtor(&data);
    return true;

    exit_mark:
    FREE(text.str_ptr);
    parsing_block_t_dtor(&data);
    return false;
}

