#ifndef BACKEND_H
#define BACKEND_H

#include "AST_proc.h"


bool BackEnd_generate_asm_code(ast_tree_t *ast_tree, const char asm_code_outpath[]);


#endif // BACKEND_H
