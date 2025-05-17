#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "general.h"
#include "graphviz_funcs.h"

bool dot_dir_ctor(dot_dir_t *dot_dir, const char dot_dir_path[], const char dot_file_name[], const char dot_img_name[]) {
    assert(dot_dir);
    memcpy(dot_dir->dot_dir, dot_dir_path, MAX_DOT_DIR_SZ);
    snprintf(dot_dir->dot_code_file_path, MAX_DOT_FILE_NAME_SZ, "%s/%s", dot_dir_path, dot_file_name);
    snprintf(dot_dir->dot_img_path, MAX_DOT_IMG_NAME_SZ, "%s/%s", dot_dir_path, dot_img_name);

    dot_dir->dot_code_file = fopen(dot_dir->dot_code_file_path, "w");
    if (dot_dir->dot_code_file == NULL) {
        debug("can't open '%s'", dot_dir->dot_code_file_path);
        return false;
    }

    return true;
}

void dot_dir_dtor(dot_dir_t *dot_dir) {
    if (!dot_dir) {
        return;
    }
    if (dot_dir->dot_code_file) {
        fclose(dot_dir->dot_code_file);
    }
}

bool dot_code_t_ctor(dot_code_t *dot_code, dot_code_pars_t pars) {
    assert(dot_code != NULL);

    (*dot_code).pars = pars;

    (*dot_code).node_list_sz = 0;

    (*dot_code).edge_list_sz = 0;

    return true;
}

void dot_code_t_dtor(dot_code_t *dot_code) {
    if (dot_code == NULL) {
        return;
    }

}

void dot_write_node(FILE *dot_code_file, dot_node_t *node) {
    assert(node != NULL);
    assert(dot_code_file != NULL);

    fprintf(dot_code_file, "    NODE%p[", node);
    if (node->pars.shape) {
        fprintf(dot_code_file, "shape=\"%s\"", node->pars.shape);
    }
    if (node->pars.color) {
        fprintf(dot_code_file, ",color=\"%s\"", node->pars.color);
    }
    if (node->pars.fillcolor) {
        fprintf(dot_code_file, ",fillcolor=\"%s\"", node->pars.fillcolor);
    }
    if (node->pars.style) {
        fprintf(dot_code_file, ",style=\"%s\"", node->pars.style);
    }
    if (node->label) {
        fprintf(dot_code_file, ",label=\"%s\"", node->label);
    }
    fprintf(dot_code_file, "]\n");
}

int dot_new_node(dot_code_t *dot_code, dot_node_pars_t pars, const char *label) {
    if (dot_code->node_list_sz >= dot_code->node_list_cap) {
        debug("node_list overflow");
        return -1;
    }
    dot_code->node_list[dot_code->node_list_sz++] = {pars, label};
    return (int) (dot_code->node_list_sz) - 1;
}

int dot_new_edge(dot_code_t *dot_code, size_t node1_idx, size_t node2_idx, dot_edge_pars_t pars, const char *label) {
    if (dot_code->edge_list_sz >= dot_code->edge_list_cap) {
        debug("edge_list overflow");
        return -1;
    }
    dot_code->edge_list[dot_code->edge_list_sz++] =
        {
            &dot_code->node_list[node1_idx],&dot_code->node_list[node2_idx],  pars, label
        };

    return (int) (dot_code->edge_list_sz) - 1;
}

void dot_start_graph(FILE *dot_code_file, dot_code_pars_t *pars) {
    assert(pars != NULL);

    fprintf(dot_code_file, "digraph G{\n");
    if (pars->rankdir) {
        fprintf(dot_code_file, "    rankdir=%s;\n", pars->rankdir);
    }
}

void dot_end_graph(FILE *dot_code_file) {
    fprintf(dot_code_file, "}");
    fclose(dot_code_file);
}

void dot_write_edge(FILE *dot_code_file, dot_edge_t *edge) {
    assert(edge != NULL);
    assert(dot_code_file != NULL);

    fprintf(dot_code_file, "    NODE%p", edge->start);
    if (edge->pars.start_suf) {
        fprintf(dot_code_file, ":%s", edge->pars.start_suf);
    }

    fprintf(dot_code_file, "-> ");

    fprintf(dot_code_file, "NODE%p", edge->end);
    if (edge->pars.end_suf) {
        fprintf(dot_code_file, ":%s", edge->pars.end_suf);
    }

    fprintf(dot_code_file, " [");


    if (edge->pars.color) {
        fprintf(dot_code_file, "color=\"%s\"", edge->pars.color);
    }
    if (edge->pars.penwidth) {
        fprintf(dot_code_file, ",penwidth=%lu", edge->pars.penwidth);
    }
    if (edge->label) {
        fprintf(dot_code_file, ",label=\"%s\"", edge->label);
    }

    fprintf(dot_code_file, "]\n");
}

void dot_code_fwrite(dot_code_t *dot_code, FILE *dotcode_outfile) {
    assert(dotcode_outfile);
    assert(dot_code);

    dot_start_graph(dotcode_outfile, &dot_code->pars);
    for (size_t i = 0; i < dot_code->node_list_sz; i++) {
        dot_write_node(dotcode_outfile, &(dot_code->node_list[i]));
    }
    for (size_t i = 0; i < dot_code->edge_list_sz; i++) {
        dot_write_edge(dotcode_outfile, &(dot_code->edge_list[i]));
    }
    dot_end_graph(dotcode_outfile);
}

bool dot_draw_image(const char ast_dotcode_outpath[], const char ast_img_outpath[]) {
    assert(ast_dotcode_outpath);
    assert(ast_dotcode_outpath);

    char draw_graph_command[BUFSIZ] = {};
    snprintf(draw_graph_command, BUFSIZ, "dot %s -Tpng -o %s",
        ast_dotcode_outpath, ast_img_outpath);

    if (system(draw_graph_command) != 0) {
        debug("system exectuion : '%s' failed", draw_graph_command);
        return false;
    }
    return true;
}

bool dot_code_create_ast_img(dot_code_t *dot_code, const char ast_img_outpath[]) {
    assert(dot_code);

    const char *ast_dotcode_real_outpath = ast_img_outpath;
    FILE * dotcode_outfile = fopen(ast_dotcode_real_outpath, "w");
    if (!dotcode_outfile) {
        debug("open `%s` failed\n", ast_dotcode_real_outpath);
        return false;
    }

    dot_code_fwrite(dot_code, dotcode_outfile);

    if (!dot_draw_image(ast_dotcode_real_outpath, ast_img_outpath)) {
        debug("dot_draw_image failed\n");
        return false;
    }

    return true;
}