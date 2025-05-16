// #include "elf_gen.hpp"
// #include "general.hpp"

// #include <stdlib.h>


// int main() {
//     elf_t elf = {};
//     fill_executable_linked_format(&elf);

//     const char ELF_FILE_OUTPATH[] = "./generated_elf.out";

//     if (!create_executable_linked_file(&elf, ELF_FILE_OUTPATH)) {
//         debug("create_executable_linked_file failed\n");
//         return EXIT_FAILURE;
//     }
// }

// #include <stdio.h>

// int main() {
//     printf("Hello, World!");
// }


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "string_funcs.h"
#include "graphviz_funcs.h"
#include "general.h"
#include "dot_log.h"


const char DOT_DIR_PATH[] = "./logs/dot";
const char DOT_FILE_NAME[] = "graph.dot";
const char DOT_IMG_NAME[] = "gr_img.png";

const dot_node_pars_t DEFAULT_NODE_PARS = {"Mrecord", "red", "white", "filled"};
const dot_edge_pars_t DEFAULT_EDGE_PARS = {NULL, NULL, "cyan", 2};
const dot_code_pars_t LIST_DOT_CODE_PARS = {"LR"};

int main() {
    // test_mode_launch();

    dot_code_t dot_code = {};
    dot_code_t_ctor(&dot_code, LIST_DOT_CODE_PARS);

    dot_new_node(&dot_code, DEFAULT_NODE_PARS, "bara");
    dot_new_node(&dot_code, DEFAULT_NODE_PARS, "bara");
    dot_new_node(&dot_code, DEFAULT_NODE_PARS, "bara");
    dot_new_node(&dot_code, DEFAULT_NODE_PARS, "bere");
    dot_code.node_list[0].pars.color = "cyan";
    printf("max: %lu\n", get_max_str_len(3, dot_code.node_list[0].pars.color, dot_code.node_list[0].pars.style, dot_code.node_list[0].pars.shape));
    dot_new_edge(&dot_code, 0, 1, DEFAULT_EDGE_PARS, ":)");
    dot_new_edge(&dot_code, 1, 2, DEFAULT_EDGE_PARS, "^_^");

    dot_dir_t dot_dir = {};
    dot_dir_ctor(&dot_dir, DOT_DIR_PATH, DOT_FILE_NAME, DOT_IMG_NAME);


    dot_code_t_dump(stdout, &dot_code);

    dot_code_render(&dot_dir, &dot_code);
}