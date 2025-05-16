#ifndef GRAPHVIZ_FUNCS_H
#define GRAPHVIZ_FUNCS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const size_t DOT_CODE_LIST_MAX_SZ = BUFSIZ;
const size_t STR_STORAGE_CHUNK_SIZE = BUFSIZ;
const size_t MAX_DOT_DIR_SZ = 128;
const size_t MAX_DOT_FILE_NAME_SZ = 128;
const size_t MAX_DOT_IMG_NAME_SZ = 128;
const size_t MAX_SYSTEM_COMMAND_SIZE = 128;

struct dot_code_pars_t {
    const char *rankdir;
};
struct dot_node_pars_t {
    const char *shape;
    const char *color;
    const char *fillcolor;
    const char *style;
};
struct dot_node_t {
    dot_node_pars_t pars;

    const char *label;
};
struct dot_edge_pars_t {
    const char *start_suf;
    const char *end_suf;

    const char *color;
    size_t penwidth;
};
struct dot_edge_t {
    void *start;
    void *end;

    dot_edge_pars_t pars;

    const char *label;
};
struct dot_dir_t {
    char dot_dir[MAX_DOT_DIR_SZ];
    char dot_code_file_path[MAX_DOT_FILE_NAME_SZ];
    char dot_img_path[MAX_DOT_IMG_NAME_SZ];

    FILE* dot_code_file;
};
struct dot_code_t {
    dot_code_pars_t pars;

    dot_node_t node_list[DOT_CODE_LIST_MAX_SZ];
    size_t node_list_cap = DOT_CODE_LIST_MAX_SZ;
    size_t node_list_sz;

    dot_edge_t edge_list[DOT_CODE_LIST_MAX_SZ];
    size_t edge_list_cap = DOT_CODE_LIST_MAX_SZ;
    size_t edge_list_sz;
};

bool dot_dir_ctor(dot_dir_t *dot_dir, const char dot_dir_path[], const char dot_file_name[], const char dot_img_name[]);
void dot_dir_dtor(dot_dir_t *dot_dir);
bool dot_code_t_ctor(dot_code_t *dot_code, dot_code_pars_t pars);
void dot_code_t_dtor(dot_code_t *dot_code);
int dot_new_node(dot_code_t *dot_code, dot_node_pars_t pars, const char *label);
int dot_new_edge(dot_code_t *dot_code, size_t node1_idx, size_t node2_idx, dot_edge_pars_t pars, const char *label);
void dot_write_node(FILE *dot_code_file, dot_node_t *node);
void dot_start_graph(FILE *dot_code_file, dot_code_pars_t *pars);
void dot_end_graph(FILE *dot_code_file);
void dot_draw_image(dot_dir_t *dot_dir);
void dot_write_edge(FILE *dot_code_file, dot_edge_t *edge);
void dot_code_render(dot_dir_t *dot_dir, dot_code_t *dot_code);

#endif // GRAPHVIZ_FUNCS_H