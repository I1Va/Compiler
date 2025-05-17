#include "BackEnd.h"
#include <assert.h>
#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#include <stdlib.h>

#include "AST_proc.h"
#include "ast_translator.h"
#include "general.h"


// const size_t CHUNK_SIZE = 1024;
// const char DOT_DIR_PATH[] = "./logs";
// const char DOT_FILE_NAME[] = "graph.dot";
// const char DOT_IMG_NAME[] = "gr_img.png";
// const char ASM_CODE_PATH[] = "./asm_code.txt";
char bufer[BUFSIZ] = {};

// bool BackEnd_generate_IR(ast_tree_t *ast_tree) {
//     assert(ast_tree);

//     translate_ast_to_asm_code();

//     assembler_make_bin_code(ASM_CODE_PATH, main_config.output_file);

//     FREE(text.str_ptr);
//     sub_tree_dtor(tree.root);
//     str_storage_t_dtor(storage);

//     return EXIT_SUCCESS;
// }

// #include <assert.h>
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>

// #include "AST_io.h"
// #include "assembler/inc/general.h"
// #include "general.h"
// #include "AST_proc.h"
// #include "string_funcs.h"
// #include "back_args_proc.h"
// #include "ast_translator.h"


// const char LOG_FILE_PATH[] = "./logs/log.html";
// const size_t CHUNK_SIZE = 1024;
// const char DOT_DIR_PATH[] = "./logs";
// const char DOT_FILE_NAME[] = "graph.dot";
// const char DOT_IMG_NAME[] = "gr_img.png";
// const char ASM_CODE_PATH[] = "./asm_code.txt";
// char bufer[BUFSIZ] = {};




//     translate_ast_to_asm_code(ASM_CODE_PATH, &tree);

//     assembler_make_bin_code(ASM_CODE_PATH, main_config.output_file);

//     convert_subtree_to_dot(tree.root, &dot_code, &storage);

//     dot_code_render(&dot_dir, &dot_code);

//     FREE(text.str_ptr);
//     sub_tree_dtor(tree.root);
//     str_storage_t_dtor(storage);

//     return EXIT_SUCCESS;
// }


bool BackEnd_generate_asm_code_from_tree_file(str_storage_t **storage, const char asm_tree_inpath[], const char asm_code_outpath[]) {
    assert(storage);
    assert(asm_tree_inpath);
    assert(asm_code_outpath);

    ast_tree_t ast_tree = {}; ast_tree_ctor(&ast_tree);
    str_t text          = read_text_from_file(asm_tree_inpath);

    ast_tree.root = load_ast_tree(text.str_ptr, storage, bufer);
    if (!text.str_ptr) {
        debug("load_ast_tree failed\n");
        CLEAR_MEMORY(exit_mark)
    }

    if (!ast_tree.root) {
        debug("load_ast_tree failed\n");
        CLEAR_MEMORY(exit_mark)

    }

    translate_ast_to_asm_code(asm_code_outpath, &ast_tree);

    free(text.str_ptr);
    return true;

    exit_mark:
    if (text.str_ptr) free(text.str_ptr);
    return false;
}

bool BackEnd_generate_asm_code(ast_tree_t *ast_tree, const char asm_code_outpath[]) {
    assert(ast_tree);
    assert(asm_code_outpath);

    translate_ast_to_asm_code(asm_code_outpath, ast_tree);

    return true;
}
