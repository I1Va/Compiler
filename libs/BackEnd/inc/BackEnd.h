#ifndef BACKEND_H
#define BACKEND_H

#include "AST_proc.h"
#include "string_funcs.h"


bool BackEnd_generate_asm_code(ast_tree_t *ast_tree, const char asm_code_outpath[]);
bool BackEnd_generate_asm_code_from_tree_file(str_storage_t **storage, const char asm_tree_inpath[], const char asm_code_outpath[]);


#endif // BACKEND_H
