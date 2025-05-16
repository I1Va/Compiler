#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "graphviz_funcs.h"
#include "string_funcs.h"
#include "dot_log.h"

void dot_code_t_dump(FILE *log_file, dot_code_t *dot_code) {
    assert(log_file != NULL);
    assert(dot_code != NULL);

    char bufer[BUFSIZ] = {};
    size_t indent_sz = 4;
    size_t indent_step = 4;

    fprintf(log_file, "dot_code_t\n{\n");

    size_t dot_code_pars_block_sz = get_max_str_len(5,
        "rankdir_",
        "node_list_cap_", "node_list_sz_",
        "edge_list_cap_", "edge_list_sz_"
    );

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "rankdir_");
    fprintf(log_file, " = '%s'\n\n", dot_code->pars.rankdir);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "node_list_cap_");
    fprintf(log_file, " = %lu\n", dot_code->node_list_cap);

    fprintf_str_block(log_file, indent_sz, dot_code_pars_block_sz, "node_list_sz_");
    fprintf(log_file, " = %lu\n", dot_code->node_list_sz);

    fprintf_indent_str(log_file, indent_sz, "node_list_ = \n");
    fprintf_indent_str(log_file, indent_sz, "{\n");

    for (size_t i = 0; i < dot_code->node_list_sz; i++) {
        fprintf_indent_str(log_file, indent_sz + indent_step, "");
        dot_write_node(log_file, &dot_code->node_list[i]);
    }
    fprintf_indent_str(log_file, indent_sz, "};\n");

    fprintf(log_file, "}\n");
}
