#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

#include "backend_args_proc.h"
#include "AST_proc.h"
#include "AST_io.h"
#include "graphviz_funcs.h"
#include "ast_translator.h"
#include "string_funcs.h"

bool file_exist(const char path[]) {
    assert(path);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

bool directory_exists(const char path[]) {
    struct stat info;

    if (stat(path, &info) != 0) {
        // stat не смог получить информацию — директория не существует
        return 0;
    } else if (info.st_mode & S_IFDIR) {
        // Это директория
        return 1;
    } else {
        // Путь существует, но это не директория
        return 0;
    }
}

int main(const int argc, const char *argv[]) {
    char bufer[BUFSIZ] = {};

    main_config_t main_config = {}; main_config_get(&main_config, argc, argv);
    ast_tree_t tree           = {}; ast_tree_ctor(&tree);
    dot_code_t dot_code       = {}; dot_code_t_ctor(&dot_code, LIST_DOT_CODE_PARS);

    str_storage_t *storage    = str_storage_t_ctor(STR_STORAGE_CHUNK_SIZE);
    str_t text                = read_text_from_file(main_config.input_file);

    tree.root = load_ast_tree(text.str_ptr, &storage, bufer);

    struct stat info = {};
    if (stat(main_config.input_file, &info) != 0) {
        MESSAGE_PRINT("can't open `%s` input file\n", main_config.input_file);
        return EXIT_SUCCESS;
    }

    if (stat(main_config.std_lib_dir, &info) != 0) {
        MESSAGE_PRINT("can't open `%s` dir with stdlib\n", main_config.std_lib_dir);
        return EXIT_SUCCESS;
    }

    translate_ast_to_asm_code(&tree, &storage, main_config.output_file, main_config.std_lib_dir);

    FREE(text.str_ptr);
    sub_tree_dtor(tree.root);
    str_storage_t_dtor(storage);

    return EXIT_SUCCESS;
}