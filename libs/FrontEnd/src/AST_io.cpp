#include <stdio.h>
#include <string.h>

#include "AST_proc.h"
#include "AST_structs.h"
#include "FrontEnd.h"
#include "general.h"
#include "assert.h"
#include "string_funcs.h"
#include "AST_io.h"
#include "graphviz_funcs.h"




const char COLORS[][16] =
{
    "#aaf3b4",
    "#f3b4aa",
    "#e3c0ab",
    "#aed768",
    "#6c3483",
    "#975abc",
    "#e26e0e",
    "#a1b5ac",
    "#8d6e63",
    "#d56050",
    "#ddd660",
    "#883060",
    "#04859D",
};
const int COLORS_CNT = sizeof(COLORS) / sizeof(COLORS[0]);

ast_tree_elem_t *load_ast_tree(char *src, str_storage_t **storage, char *bufer) {
    assert(src);

    static char *text = src;

    text = strchr(text, '{');
    if (text == NULL) {
        debug("'{' lexem hasn't been found");
        return NULL;
    }

    text++;

    ast_tree_elem_value_t node_val = {};
    int left_son_exists = 0;
    int right_son_exists = 0;


    sscanf
    (
        text, "%d %ld %lf %s %d %d",
        &node_val.ast_node_type,
        &node_val.value.int64_val, &node_val.value.double_val, bufer,
        &left_son_exists, &right_son_exists
    );

    // printf
    // (
    //     "node: '%d %d %Ld %Lg %s %d %d'\n",
    //     node_val.type,
    //     node_val.value.ival, node_val.value.lval, node_val.value.fval, bufer,
    //     left_son_exists, right_son_exists
    // );


    if (strcmp(bufer, "\\0") == 0) {
        node_val.value.sval = NULL;
    } else {
        size_t buf_len = strlen(bufer);
        node_val.value.sval = get_new_str_ptr(storage, buf_len);
        strncpy(node_val.value.sval, bufer, buf_len);
    }

    ast_tree_elem_t *node = ast_tree_create_node(NULL, NULL, node_val);

    if (left_son_exists) {
        node->left = load_ast_tree(text, storage, bufer);
    }
    if (right_son_exists) {
        node->right = load_ast_tree(text, storage, bufer);
    }

    return node;
}

void ast_tree_file_dump_rec(FILE* stream, ast_tree_elem_t *node, size_t indent) {
    if (!node) {
        return;
    }
    fprintf_indent_str(stream, indent, "");
    // fprintf_n_chars(stream, ' ', indent);
    fprintf(stream, "{");

    char *outp_sval = "\\0";
    if (node->data.value.sval) {
        outp_sval = node->data.value.sval;
    }

    fprintf
    (
        stream, "%d %ld %f %s %d %d\n",
        node->data.ast_node_type,
        node->data.value.int64_val, node->data.value.double_val, outp_sval,
        node->left != NULL, node->right != NULL
    );

    if (node->left) {
        ast_tree_file_dump_rec(stream, node->left, indent + 4);
    }
    if (node->right) {
        ast_tree_file_dump_rec(stream, node->right, indent + 4);
    }
    fprintf_indent_str(stream, indent, "");
    // fprintf_n_chars(stream, ' ', indent);
    fprintf(stream, "}\n");
}

void ast_tree_file_dump(const char path[], ast_tree_t *tree, size_t indent) {
    assert(path != NULL);
    assert(tree != NULL);

    FILE *tree_file_ptr = fopen(path, "wb");
    if (tree_file_ptr == NULL) {
        printf("file '%s' open failed", path);
        return;
    }

    ast_tree_file_dump_rec(tree_file_ptr, tree->root, indent);

    if (fclose(tree_file_ptr)) {
        debug("file '%s' close failed", path);
    }
}

void get_AST_OPERATION_string(char *bufer, ast_tree_elem_t *node) {
    assert(bufer);
    assert(node);

    if (node->data.ast_node_type != AST_OPERATION) {
        snprintf(bufer, BUFSIZ, "IT'S NOT NODE_OP");
        return;
    }


    switch (node->data.value.int64_val) {
        case TOKEN_ADD: snprintf(bufer, BUFSIZ, "+"); return;
        case TOKEN_DIV: snprintf(bufer, BUFSIZ, "/"); return;
        case TOKEN_SUB: snprintf(bufer, BUFSIZ, "-"); return;
        case TOKEN_MUL: snprintf(bufer, BUFSIZ, "*"); return;

        case TOKEN_LESS_EQ: snprintf(bufer, BUFSIZ, "lesseq"); return;
        case TOKEN_LESS: snprintf(bufer, BUFSIZ, "less"); return;
        case TOKEN_MORE: snprintf(bufer, BUFSIZ, "more"); return;
        case TOKEN_MORE_EQ: snprintf(bufer, BUFSIZ, "moreq"); return;

        default: snprintf(bufer, BUFSIZ, "OP_?(%ld)", node->data.value.int64_val); return;
    }
}

void get_AST_TYPE_string(char *bufer, ast_tree_elem_t *node) {
    assert(bufer);
    assert(node);

    if (node->data.ast_node_type != AST_TYPE) {
        snprintf(bufer, BUFSIZ, "IT'S NOT AST_TYPE");
        return;
    }

    switch (node->data.value.int64_val) {
        case (TOKEN_INT64_KEYWORD): snprintf(bufer, BUFSIZ, "int64"); return;
        case (TOKEN_DOUBLE_KEYWORD): snprintf(bufer, BUFSIZ, "double"); return;
        case (TOKEN_STRING_KEYWORD): snprintf(bufer, BUFSIZ, "string"); return;
         case (TOKEN_VOID): snprintf(bufer, BUFSIZ, "void"); return;
        default: snprintf(bufer, BUFSIZ, "unknown type (%ld)", node->data.value.int64_val); return;
    }
}

void get_node_string(char *bufer, ast_tree_elem_t *node) {
    assert(bufer);

    if (node == NULL) {
        snprintf(bufer, BUFSIZ, "NULL");
        return;
    }

    switch (node->data.ast_node_type) {
        case AST_FUNC_INIT:     snprintf(bufer, BUFSIZ, "func init"); return;
        case AST_VAR_INIT:      snprintf(bufer, BUFSIZ, "var init"); return;
        case AST_VAR_ID:        snprintf(bufer, BUFSIZ, "var_id: '%s'", node->data.value.sval); return;
        case AST_FUNC_ID:       snprintf(bufer, BUFSIZ, "func_id: '%s'", node->data.value.sval); break;
        case AST_ASSIGN:        snprintf(bufer, BUFSIZ, "="); return;
        case AST_NUM_INT64:     snprintf(bufer, BUFSIZ, "%ld", node->data.value.int64_val); return;
        case AST_NUM_DOUBLE:    snprintf(bufer, BUFSIZ, "%lf", node->data.value.double_val); return;
        case AST_STR_LIT:       snprintf(bufer, BUFSIZ, "`string_value`"); return;
        case AST_RETURN:        snprintf(bufer, BUFSIZ, "return"); return;
        case AST_BREAK:         snprintf(bufer, BUFSIZ, "break"); return;
        case AST_CONTINUE:      snprintf(bufer, BUFSIZ, "continue"); return;
        case AST_CALL:          snprintf(bufer, BUFSIZ, "call"); return;
        case AST_ELSE:          snprintf(bufer, BUFSIZ, "else"); return;
        case AST_SCOPE:         snprintf(bufer, BUFSIZ, "scope"); return;
        case AST_WHILE:         snprintf(bufer, BUFSIZ, "while"); return;
        case AST_SEMICOLON:     snprintf(bufer, BUFSIZ, ";"); return;
        case AST_IF:            snprintf(bufer, BUFSIZ, "if"); return;
        case AST_COMMA:         snprintf(bufer, BUFSIZ, ","); return;
        case AST_TYPE:          get_AST_TYPE_string(bufer, node); return;
        case AST_OPERATION:     get_AST_OPERATION_string(bufer, node); return;

        default: snprintf(bufer, BUFSIZ, "UNKNOWN_NODE_TYPE(%d)", node->data.ast_node_type);
    }
}

size_t seg_char_cnt(char *left, char *right, char c) {
    size_t cnt = 0;

    while (left <= right) {
        cnt += (*left++ == c);
    }

    return cnt;
}

void fprintf_seg(FILE *stream, char *left, char *right) {
    for (;left <= right; left++) {
        fputc(*left, stream);
    }
}

int put_node_in_dotcode(ast_tree_elem_t *node, dot_code_t *dot_code, str_storage_t **storage) {
    assert(dot_code != NULL);
    assert(storage != NULL);
    assert(node != NULL);

    char bufer[MEDIUM_BUFER_SZ] = {};
    get_node_string(bufer, node);

    size_t label_sz = MAX_NODE_WRAP_SZ + strlen(bufer);
    char *label = get_new_str_ptr(storage, label_sz);
    snprintf(label, label_sz, "{'%s' | {<L> (L) | <R> (R)}}", bufer);


    int node_idx = dot_new_node(dot_code, DEFAULT_NODE_PARS, label);
    // printf("label : [%s], node_idx: {%d}\n", label, node_idx);

    dot_code->node_list[node_idx].pars.fillcolor = COLORS[node->data.ast_node_type % COLORS_CNT];

    return node_idx;
}

int convert_subtree_to_dot(ast_tree_elem_t *node, dot_code_t *dot_code, str_storage_t **storage) {
    assert(dot_code != NULL);
    assert(storage != NULL);
    assert(node != NULL);

    node->graphviz_idx = put_node_in_dotcode(node, dot_code, storage);

    int left_son_idx = -1;
    int right_son_idx = -1;

    if (node->left) {
        left_son_idx = convert_subtree_to_dot(node->left, dot_code, storage);
        if (left_son_idx == -1) {
            debug("dot_code overflow");
            return -1;
        }
    }
    if (node->right) {
        right_son_idx = convert_subtree_to_dot(node->right, dot_code, storage);
        if (right_son_idx == -1) {
            debug("dot_code overflow");
            return -1;
        }
    }

    if (left_son_idx != -1) {
        int left_edge_idx = dot_new_edge(dot_code, (size_t) node->graphviz_idx, (size_t) left_son_idx, DEFAULT_EDGE_PARS, "");
        if (left_edge_idx == -1) {
            debug("dot_code overflow");
            return -1;
        }
        dot_code->edge_list[left_edge_idx].pars.start_suf = "L";
    }
    if (right_son_idx != -1) {
        int right_edge_idx = dot_new_edge(dot_code, (size_t) node->graphviz_idx, (size_t) right_son_idx, DEFAULT_EDGE_PARS, "");
        if (right_edge_idx == -1) {
            debug("dot_code overflow");
            return -1;
        }
        dot_code->edge_list[right_edge_idx].pars.start_suf = "R";
    }

    return (int) node->graphviz_idx;
}

void node_dump(FILE *log_file, ast_tree_elem_t *node) {
    assert(log_file != NULL);
    assert(node != NULL);

    char bufer[BUFSIZ] = {};
    size_t indent_sz = 4;
    size_t indent_step = 4;

    fprintf(log_file, "node[%p]\n{\n", node);

    size_t dot_code_pars_block_sz = get_max_str_len(5,
        "left_", "right_",
        "prev_", "is_left_son_",
        "data_"
    );

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "left_");
    get_node_string(bufer, node->left);
    fprintf(log_file, " = ([%p]; '%s')\n", node->left, bufer);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "right_");
    get_node_string(bufer, node->right);
    fprintf(log_file, " = ([%p]; '%s')\n", node->right, bufer);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "prev_");
    get_node_string(bufer, node->prev);
    fprintf(log_file, " = ([%p]; '%s')\n", node->prev, bufer);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "is_left_son_");
    fprintf(log_file, " = (%d)\n", node->is_node_left_son);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "data_");
    get_node_string(bufer, node);
    fprintf(log_file, " = ('%s')\n", bufer);

    fprintf(log_file, "}\n");
}