#include <assert.h>
#include <cmath>
#include <stdlib.h>

#include "AST_io.h"
#include "AST_proc.h"
#include "graphviz_funcs.h"
#include "string_funcs.h"
#include "front_args_proc.h"
#include "front_args_proc.h"
#include "lang_lexer.h"


int main(const int argc, const char *argv[]) {
    main_config_t main_config = {}; main_config_get(&main_config, argc, argv);
    ast_tree_t tree           = {}; ast_tree_ctor(&tree);
    dot_code_t dot_code       = {}; dot_code_t_ctor(&dot_code, LIST_DOT_CODE_PARS);
    str_storage_t *storage    = str_storage_t_ctor(STR_STORAGE_CHUNK_SIZE);

    FrontEnd_make_AST(&tree, &storage, main_config.input_file, main_config.output_file, main_config.ast_img_outpath);

    dot_code_t_dtor(&dot_code);
    ast_tree_dtor(&tree);
    str_storage_t_dtor(storage);
}

