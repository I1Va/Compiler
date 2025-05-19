#ifndef AST_PROC_H
#define AST_PROC_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "AST_structs.h"

// ERROR_PROCESSING \\-------------------------------------------------------------------------------->

enum ast_tree_err_t {
    BT_ERR_OK                       = 0ull,
    BT_ERR_FILE_OPEN                = 1ull << 0,
    BT_ERR_ALLOC                    = 1ull << 1,
    BT_ERR_SYSTEM                   = 1ull << 2,
    BT_ERR_CYCLED                   = 1ull << 4,
};

const size_t ERR_DESCR_MAX_SIZE = 128;
const size_t MAX_NODES_CNT = 1ull << 17;

void ast_tree_err_get_descr(enum ast_tree_err_t err_code, char err_descr_str[]);
void ast_tree_err_add(enum ast_tree_err_t *dest, enum ast_tree_err_t add);

#ifndef NDEBUG
    #define DEBUG_BT_LIST_ERROR(err_code, str_, ...) {                                                      \
        char BT_list_err_desr_str[ERR_DESCR_MAX_SIZE];                                                    \
        ast_tree_err_get_descr(err_code, BT_list_err_desr_str); \
        printf("DESCR: '%s'\n", BT_list_err_desr_str);                                                      \
        fprintf_red(stderr, "{%s} [%s: %d]: err_descr: {%s}, message: {" str_ "}\n",              \
             __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, BT_list_err_desr_str, ##__VA_ARGS__);                    \
        fprintf(stderr, WHT); \
    }

    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define DEBUG_BT_LIST_ERROR(err_code, str_, ...) ;
#endif // NDEBUG


// TREE_PROCESSING \\-------------------------------------------------------------------------------->

const size_t MAX_NODE_STRING_SZ = 128;
const size_t NODE_LABEL_MAX_SZ = 128;

const char VALID_OPERATIONS[] = "+/*-";
const size_t VALID_OPERATIONS_CNT = strlen(VALID_OPERATIONS);
const size_t MAX_NODE_WRAP_SZ = 64;

const size_t AST_TREE_DUMP_INDENT = 4;

void ast_tree_ctor(ast_tree_t *tree);
ast_tree_elem_t *ast_tree_create_node(ast_tree_elem_t *left, ast_tree_elem_t *right, const ast_tree_elem_value_t data);
void ast_tree_print(ast_tree_elem_t *node, void (*outp_func)(char *dest, const size_t maxn_n, const ast_tree_elem_t *node));
void ast_tree_rec_nodes_cnt(ast_tree_elem_t *node, size_t *nodes_cnt);
void ast_tree_verify(const ast_tree_t tree, ast_tree_err_t *return_err);
ast_tree_elem_t *get_node_copy(ast_tree_elem_t *node);
ast_tree_elem_t *get_msubtree_copy(ast_tree_elem_t *root);
void sub_tree_dtor(ast_tree_elem_t *root);
void mark_subtree(ast_tree_elem_t *root, ast_tree_t *tree);
void ast_tree_dtor(ast_tree_t *tree);
size_t place_subtrees_sz(ast_tree_elem_t *root);
size_t count_node_type_in_subtreeas(ast_tree_elem_t *node, const enum ast_node_types node_type);


#endif // AST_PROC_H