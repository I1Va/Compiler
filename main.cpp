#include <assert.h>
#include <cstdlib>
#include <stdlib.h>

#include "AST_proc.h"
#include "FrontEnd.h"
#include "BackEnd.h"

#include "general.h"
#include "string_funcs.h"


// GENERAL
const char HIGH_LEVEL_CODE_PATH[] = "./testing_space/code.mcc";
const char AST_TREE_OUTPATH[] = "./testing_space/AST_tree.txt";
const char AST_IMG_OUTPATH[] = "./testing_space/AST_img.png";
const char ASM_CODE_OUTPATH[] = "./testing_space/code.txt";

int main() {
    ast_tree_t ast_tree    =  {}; ast_tree_ctor(&ast_tree);
    str_storage_t *storage =  str_storage_t_ctor(STR_FUNCS_FRONTEND_CHUNK_SIZE);

    if (!FrontEnd_make_AST(&ast_tree, &storage, HIGH_LEVEL_CODE_PATH, AST_TREE_OUTPATH, AST_IMG_OUTPATH)) {
        debug("FrontEnd_make_AST failed\n");
        CLEAR_MEMORY(exit_mark)
        return EXIT_FAILURE;
    }

    if (!BackEnd_generate_asm_code(&ast_tree, &storage, ASM_CODE_OUTPATH)) {
        debug("BackEnd_generate_asm_code failed\n");
        CLEAR_MEMORY(exit_mark)
    }


    sub_tree_dtor(ast_tree.root);
    str_storage_t_dtor(storage);
    return EXIT_SUCCESS;

    exit_mark:
    sub_tree_dtor(ast_tree.root);
    str_storage_t_dtor(storage);
    return EXIT_FAILURE;
}
