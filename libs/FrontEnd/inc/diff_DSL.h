#ifndef DIFF_DSL_H
#define DIFF_DSL_H

#include "AST_structs.h"

const multi_val_t EMPTY_MULTI_VAL = {};

#define _RETURN(return_node)           ast_tree_create_node(return_node, NULL,    {AST_RETURN,      EMPTY_MULTI_VAL})
#define _BREAK()                       ast_tree_create_node(NULL, NULL,           {AST_BREAK,       EMPTY_MULTI_VAL})
#define _CONTINUE()                    ast_tree_create_node(NULL, NULL,           {AST_CONTINUE,    EMPTY_MULTI_VAL})
#define _ASSIGN(var, val)              ast_tree_create_node(var, val,             {AST_ASSIGN,      EMPTY_MULTI_VAL})
#define _VAR_INIT(type, var)           ast_tree_create_node(type, var,            {AST_VAR_INIT,    EMPTY_MULTI_VAL})
#define _FUNC_INIT(type, var)          ast_tree_create_node(type, var,            {AST_FUNC_INIT,   EMPTY_MULTI_VAL})
// #define _GLOBAL(gl_states)             ast_tree_create_node(gl_states, NULL,      {AST_GLOBAL,    EMPTY_MULTI_VAL})
#define _CALL(func_id, args)           ast_tree_create_node(func_id, args,        {AST_CALL,        EMPTY_MULTI_VAL})
#define _ELSE(if_body, else_body)      ast_tree_create_node(if_body, else_body,   {AST_ELSE,        EMPTY_MULTI_VAL})
#define _SCOPE(statement_list)         ast_tree_create_node(statement_list, NULL, {AST_SCOPE,       EMPTY_MULTI_VAL})
#define _WHILE(expr, body)             ast_tree_create_node(expr, body,           {AST_WHILE,       EMPTY_MULTI_VAL})
#define _SEMICOLON(left, right)        ast_tree_create_node(left, right,          {AST_SEMICOLON,   EMPTY_MULTI_VAL})
#define _IF(cond, else_node)           ast_tree_create_node(cond, else_node,      {AST_IF,          EMPTY_MULTI_VAL})
#define _COMMA(copy_node, args_node)   ast_tree_create_node(copy_node, args_node, {AST_COMMA,       EMPTY_MULTI_VAL})

#define _OP(num, left, right)          ast_tree_create_node(left, right,          {AST_OPERATION,   {num, 0.0f, NULL}})
#define _TYPE(num)                     ast_tree_create_node(NULL, NULL,           {AST_TYPE,        {num, 0.0f, NULL}})

#define _NUM_INT64(val)                ast_tree_create_node(NULL, NULL,           {AST_NUM_INT64,   {val, 0.0f, NULL}})
#define _NUM_DOUBLE(val)               ast_tree_create_node(NULL, NULL,           {AST_NUM_DOUBLE,  {0l, val, NULL}})

#define _FUNC_ID(name, name_id)        ast_tree_create_node(NULL, NULL,           {AST_FUNC_ID,     {name_id, 0.0f, name}})
#define _STR_LIT(str, len)             ast_tree_create_node(NULL, NULL,           {AST_STR_LIT,     {len, 0.0f, str}})
#define _VAR(name, name_table_idx)     ast_tree_create_node(NULL, NULL,           {AST_VAR_ID,      {name_table_idx, 0.0f, name}})

#endif // DIFF_DSL_H