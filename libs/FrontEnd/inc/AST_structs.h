#ifndef AST_STRUCTS_H
#define AST_STRUCTS_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>


enum ast_node_types {
    AST_FUNC_INIT,
    AST_VAR_INIT,

    AST_VAR_ID,
    AST_FUNC_ID,

    AST_ASSIGN,

    AST_NUM_INT64,
    AST_NUM_DOUBLE,
    AST_STR_LIT,

    AST_RETURN,
    AST_BREAK,
    AST_CONTINUE,

    // AST_GLOBAL,
    AST_CALL,
    AST_ELSE,
    AST_SCOPE,
    AST_WHILE,
    AST_SEMICOLON,
    AST_IF,
    AST_COMMA,

    AST_TYPE, // float|int64|string keyword

    AST_OPERATION,
};

struct multi_val_t {
    int64_t      int64_val;
    double  double_val;
    char        *sval;
};

struct ast_tree_elem_value_t {
    enum ast_node_types ast_node_type;
    multi_val_t value;
};

struct ast_tree_elem_t {
    ast_tree_elem_t *prev;
    bool is_node_left_son;

    ast_tree_elem_t *left;
    ast_tree_elem_t *right;

    ast_tree_elem_value_t data;

    void *tree;
    bool constant_state;
    int graphviz_idx;
};

struct ast_tree_t {
    ast_tree_elem_t *root;
    FILE *log_file_ptr;
    size_t n_nodes;
};


#endif // AST_STRUCTS_H