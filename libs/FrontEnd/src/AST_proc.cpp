#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdlib.h>

#include "AST_proc.h"
#include "general.h"



// ERROR_PROCESSING \\-------------------------------------------------------------------------------->

void ast_tree_err_add(enum ast_tree_err_t *dest, enum ast_tree_err_t add) {
    *dest = (ast_tree_err_t)((unsigned long long)(*dest) | (unsigned long long) add);
}

void ast_tree_err_get_descr(const enum ast_tree_err_t err_code, char err_descr_str[]) {
    bool error = false;
    #define DESCR_(err_code, err)                 \
        {                                         \
            if (err_code & err) {                 \
                sprintf(err_descr_str, #err", "); \
                error = true;                     \
            }                                     \
        }                                         \

    DESCR_(err_code, BT_ERR_FILE_OPEN);
    DESCR_(err_code, BT_ERR_ALLOC);
    DESCR_(err_code, BT_ERR_SYSTEM);
    DESCR_(err_code, BT_ERR_CYCLED);

    if (!error) {
        sprintf(err_descr_str, "ALL IS OK:)");
    }
    #undef DESCR_
}


// TREE_PROCESSING \\-------------------------------------------------------------------------------->

void ast_tree_ctor(ast_tree_t *tree) {
    assert(tree);
    tree->log_file_ptr = NULL;
    tree->root = NULL;
    tree->n_nodes = 0;
}

ast_tree_elem_t *ast_tree_create_node(ast_tree_elem_t *left, ast_tree_elem_t *right, const ast_tree_elem_value_t data) {
    ast_tree_elem_t *node = (ast_tree_elem_t *) calloc(1, sizeof(ast_tree_elem_t));
    if (node == NULL) {
        DEBUG_BT_LIST_ERROR(BT_ERR_ALLOC, "node alloc failed");
        return NULL;
    }


    node->left = left;
    node->right = right;
    node->data = data;
    node->constant_state = false;
    node->graphviz_idx = -1;

    if (left) {
        left->prev = node;
        left->is_node_left_son = true;
    }
    if (right) {
        right->prev = node;
        right->is_node_left_son = false;
    }

    return node;
}

ast_tree_elem_t *get_node_copy(ast_tree_elem_t *node) {
    assert(node != NULL);

    ast_tree_elem_t *new_node = ast_tree_create_node(NULL, NULL, node->data);
    new_node->is_node_left_son = node->is_node_left_son;
    new_node->constant_state = node->constant_state;

    return new_node;
}

ast_tree_elem_t *get_msubtree_copy(ast_tree_elem_t *root) {
    assert(root != NULL);

    ast_tree_elem_t *root_copy  = NULL;
    ast_tree_elem_t *left_copy  = NULL;
    ast_tree_elem_t *right_copy = NULL;

    if (root->left) {
        left_copy = get_msubtree_copy(root->left);
    }
    if (root->right) {
        right_copy = get_msubtree_copy(root->right);
    }

    root_copy = get_node_copy(root);

    root_copy->left = left_copy;
    root_copy->right = right_copy;

    return root_copy;
}

size_t count_node_type_in_subtrees(ast_tree_elem_t *node, const enum ast_node_types node_type) {
    if (!node) return 0;

    size_t count = (node->data.ast_node_type == node_type);

    if (node->left) count += count_node_type_in_subtrees(node->left, node_type);

    if (node->right) count += count_node_type_in_subtrees(node->right, node_type);

    return count;
}

void ast_tree_print(ast_tree_elem_t *node, void (*outp_func)(char *dest, const size_t maxn_n, const ast_tree_elem_t *node)) {
    if (!node) {
        return;
    }
    printf("(");

    if (node->left) {
        ast_tree_print(node->left, outp_func);
    }
    char node_label[NODE_LABEL_MAX_SZ] = {};
    outp_func(node_label, NODE_LABEL_MAX_SZ, node);
    printf("%s", node_label);

    if (node->right) {
        ast_tree_print(node->right, outp_func);
    }

    printf(")");
}

void sub_tree_dtor(ast_tree_elem_t *root) {
    if (!root) {
        return;
    }

    if (root->left) {
        sub_tree_dtor(root->left);
    }
    if (root->right) {
        sub_tree_dtor(root->right);
    }
    FREE(root);
}

void ast_tree_dtor(ast_tree_t *tree) {
    if (!tree) {
        return;
    }

    sub_tree_dtor(tree->root);

    tree->root = NULL;
    tree->n_nodes = 0;
}

void ast_tree_rec_nodes_cnt(ast_tree_elem_t *node, size_t *nodes_cnt) {
    if (!node) {
        return;
    }
    if (*nodes_cnt > MAX_NODES_CNT) {
        return;
    }

    (*nodes_cnt)++;
    if (node->left) {
        ast_tree_rec_nodes_cnt(node->left, nodes_cnt);
    }
    if (node->right) {
        ast_tree_rec_nodes_cnt(node->right, nodes_cnt);
    }
}

void ast_tree_verify(const ast_tree_t tree, ast_tree_err_t *return_err) {
    size_t nodes_cnt = 0;
    ast_tree_rec_nodes_cnt(tree.root, &nodes_cnt);
    if (nodes_cnt > MAX_NODES_CNT) {
        ast_tree_err_add(return_err, BT_ERR_CYCLED);
        debug("tree might be cycled. nodes cnt exceeds max nodes cnt value");
        return;
    }
}