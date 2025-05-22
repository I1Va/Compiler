#include "backend_args_proc.h"
#include "AST_proc.h"
#include "AST_io.h"
#include "graphviz_funcs.h"
#include "ast_translator.h"


int main(const int argc, const char *argv[]) {
    char bufer[BUFSIZ] = {};

    main_config_t main_config = {}; main_config_get(&main_config, argc, argv);
    ast_tree_t tree           = {}; ast_tree_ctor(&tree);
    dot_code_t dot_code       = {}; dot_code_t_ctor(&dot_code, LIST_DOT_CODE_PARS);

    str_storage_t *storage    = str_storage_t_ctor(STR_STORAGE_CHUNK_SIZE);
    str_t text                = read_text_from_file(main_config.input_file);

    tree.root = load_ast_tree(text.str_ptr, &storage, bufer);

    translate_ast_to_asm_code(&tree, &storage, main_config.output_file);

    FREE(text.str_ptr);
    sub_tree_dtor(tree.root);
    str_storage_t_dtor(storage);

    return EXIT_SUCCESS;
}