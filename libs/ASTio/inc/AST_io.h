#ifndef AST_IO_H
#define AST_IO_H

#include "AST_proc.h"
#include "string_funcs.h"
#include "graphviz_funcs.h"

const dot_node_pars_t DEFAULT_NODE_PARS = {"Mrecord", "black", "#FFEEEE", "filled"};
const dot_edge_pars_t DEFAULT_EDGE_PARS = {NULL, NULL, "#00FF00", 2};
const dot_code_pars_t LIST_DOT_CODE_PARS = {"TB"};

enum ast_token_t {
    AST_EOF = -1,
    AST_EMPTY = 0,

    AST_NUM = 1,

    AST_O_BRACE = 7,
    AST_C_BRACE = 8,
    AST_O_FIG_BRACE = 9,
    AST_C_FIG_BRACE = 10,

    AST_ADD = 2,
    AST_MUL = 3,
    AST_SUB = 4,
    AST_DIV = 5,
    AST_POW = 6,
    AST_MORE = 17,
    AST_LESS = 18,
    AST_MORE_EQ = 19,
    AST_LESS_EQ = 20,
    AST_EQ = 21,

    AST_EOL = 11, // '\n'

    AST_SPACE = 12,


    AST_ID = 13,
    AST_IF = 14, // key_words
    AST_WHILE = 15,
    AST_SEMICOLON = 16,


    AST_INT = 22,
    AST_FLOAT = 23,
    AST_VOID = 24,
    AST_ASSIGN = 25,
    AST_COMMA = 26,
    AST_ELSE = 27,

    AST_RETURN = 28,
    AST_BREAK = 29,
    AST_CONTINUE = 30,
    AST_STR_LIT = 31,
};

enum node_types {
    NODE_EMPTY = 0,
    NODE_VAR = 1,
    NODE_NUM = 2,
    NODE_OP = 3,
    NODE_TYPE = 5,
    NODE_ASSIGN = 6,
    NODE_VAR_INIT = 7,
    NODE_FUNC_ID = 8,
    NODE_CALL = 11,
    NODE_ELSE = 12,
    NODE_SCOPE = 13,
    NODE_RETURN = 14,
    NODE_BREAK = 15,
    NODE_CONTINUE = 16,
    NODE_WHILE = 17,
    NODE_FUNC_INIT = 18,
    NODE_SEMICOLON = 19,
    NODE_IF = 20,
    NODE_COMMA = 21,
    NODE_STR_LIT = 22,
};

ast_tree_elem_t *load_ast_tree(char *text, str_storage_t **storage, char *bufer);
void ast_tree_file_dump_rec(FILE* stream, ast_tree_elem_t *node, size_t indent);
void ast_tree_file_dump(const char path[], ast_tree_t *tree, size_t indent);

void get_node_type(enum node_types *type, long double *value, char *name);
void get_node_string(char *bufer, ast_tree_elem_t *node);
size_t seg_char_cnt(char *left, char *right, char c);
void fprintf_seg(FILE *stream, char *left, char *right);
int put_node_in_dotcode(ast_tree_elem_t *node, dot_code_t *dot_code, str_storage_t **storage);
int convert_subtree_to_dot(ast_tree_elem_t *node, dot_code_t *dot_code, str_storage_t **storage);
void node_dump(FILE *log_file, ast_tree_elem_t *node);

#endif // AST_IO_H