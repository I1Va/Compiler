#include <assert.h>
#include <cstdio>
#include <string.h>

#include "AST_proc.h"
#include "ast_translator.h"
#include "stack_frame_proc.h"
#include "AST_io.h"
#include "stack_funcs.h"

#define CHECK_NODE_TYPE(node, exp_type)                                            \
    if (node->data.type != exp_type) {                                             \
        RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.type) \
    }

#define RAISE_TR_ERROR(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();


void translator_global_space_init(asm_glob_space *gl_space) {
    assert(gl_space);

    gl_space->cur_scope_deep = 0;
    gl_space->cur_frame_ptr  = 0;
    gl_space->while_counter  = 0;
    gl_space->if_counter     = 0;
    gl_space->func_init      = false;
    gl_space->void_func      = false;
    gl_space->cond_stack     = {};
    gl_space->var_stack      = {};

    STACK_INIT(&gl_space->cond_stack, 0, sizeof(int), NULL, NULL);
    STACK_INIT(&gl_space->var_stack, 0, sizeof(var_t), NULL, NULL);
}

void translator_global_space_clear(asm_glob_space *gl_space) {
    assert(gl_space);

    stack_destroy(&gl_space->var_stack);
    stack_destroy(&gl_space->cond_stack);
}

void translate_ast_to_asm_code(ast_tree_t *tree) {
    assert(tree);

    asm_glob_space gl_space = {}; translator_global_space_init(&gl_space);
    asm_payload_t asm_payload = {};
    translate_node_to_asm_code(tree->root, &gl_space, &asm_payload);

    translator_global_space_clear(&gl_space);

    // fprintf(asm_code_ptr,
    //                     "call main:\n"
    //                     "hlt;\n");
}

void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);

    switch (node->data.type) {
        case NODE_SEMICOLON: translate_semicolon(node, gl_space, asm_payload);
            break;
        // case NODE_VAR_INIT: translate_var_init(node);
        //     break;
        // case NODE_FUNC_INIT: func_init = true; translate_function_init(node); func_init = false;
        //     break;



        // case NODE_EMPTY: RAISE_TR_ERROR("incorrect AST: <NODE_EMPTY>")
        //     break;
        // case NODE_VAR: translate_var(node);
        //     break;
        // case NODE_NUM: translate_num(node);
        //     break;
        // case NODE_OP: translate_op(node);
        //     break;
        // case NODE_TYPE: fprintf_red(stdout, "there is should be <NODE_TYPE> translation\n");
        //     break;
        // case NODE_ASSIGN: translate_assign(node);
        //     break;
        // case NODE_FUNC_ID: RAISE_TR_ERROR(
        //     "<NODE_FUNC_ID> should be processed in"
        //     "<translate_func_call/translate_function_init>")
        //     break;
        // case NODE_CALL: translate_func_call(node);
        //     break;
        // case NODE_ELSE: RAISE_TR_ERROR(
        //     "<NODE_ELSE> should be processed in"
        //     "<translate_if>")
        //     break;
        // case NODE_SCOPE: translate_scope(node);
        //     break;
        // case NODE_RETURN: translate_return(node);
        //     break;
        // case NODE_BREAK: fprintf_red(stdout, "there is should be <NODE_BREAK> translation");
        //     break;
        // case NODE_CONTINUE: fprintf_red(stdout, "there is should be <NODE_CONTINUE> translation");
        //     break;
        // case NODE_WHILE: translate_while(node);
        //     break;
        // case NODE_IF: translate_if(node);
        //     break;
        // case NODE_COMMA: RAISE_TR_ERROR(
        //     "<NODE_COMMA> should be processed in"
        //     "<translate_func_call>/<translate_func_args_init>")
        //     break;
        // case NODE_STR_LIT: translate_string_literal(node);
        //     break;
        // default: RAISE_TR_ERROR("incorrect AST: <UNKNOWN_NODE(%d)>", node->data.type)
        //     break;
    }
}

void translate_semicolon(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, NODE_SEMICOLON);

    if (node->left) {
        translate_node_to_asm_code(node->left, gl_space, asm_payload);
    }
    if (node->right) {
        translate_node_to_asm_code(node->right, gl_space, asm_payload);
    }
}

// void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
//     assert(node);
//     assert(gl_space);
//     assert(asm_payload);
//     CHECK_NODE_TYPE(node, NODE_VAR_INIT);

//     bool with_assignment    = (node->right->data.type == NODE_ASSIGN);
//     bool global_state       = (gl_space->cur_scope_deep == 0);
//     int  var_type           = node->left->data.type;




//     if (global_state) {
//         if (var_type == NODE_)
//         asm_payload
//     }









//     ast_tree_elem_t *name_node = node->right;
//     const char *REG_PLUS = FRAME_PTR_REG_PLUS;
//     var_info.type    = node->left->data.type;
//     var_info.deep    = cur_scope_deep;

//     var_info.name_id = name_node->left->data.value.ival;
//     var_info.name    = name_node->left->data.value.sval;
//     var_info.global  = (var_info.deep == 0 && !func_init);
//     var_info.loc_addr = add_var_into_frame(var_info);
//     if (name_node->data.type == NODE_ASSIGN) {
//         with_assignment = true;
//         translate_node_to_asm_code(name_node->right); // push right part of assignment
//     }

//     fprintf(asm_code_ptr,
//                         "; // '%s' init, loc_addr: %d\n"
//                         "push rsp;\n"
//                         "push 1;\n"
//                         "add;\n"
//                         "pop rsp; stack_ptr++\n",
//                         var_info.name, var_info.loc_addr);

//     if (var_info.global) {REG_PLUS = "";}

//     if (with_assignment) {
//         fprintf(asm_code_ptr,
//                             ";#====Init_Var=Assinment===#\n"
//                             "pop [%s%d]; // '%s' assinment\n"
//                             ";#======End=Assinment======#\n",
//                             REG_PLUS, var_info.loc_addr, var_info.name);
//     }
// }

// FIXME:
// void translate_var(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
//     assert(node);
//     assert(gl_space);
//     assert(asm_payload);
//     CHECK_NODE_TYPE(node, NODE_VAR);

//     char *var_name = node->data.value.sval;             // строка хранится в string_storage, определенном в main.cpp
//     int var_name_id = node->data.value.ival;            // идентификатор стэкового фрэйма
//     var_t found_var = get_var_from_frame(var_name_id, &gl_space->var_stack, gl_space->cur_frame_ptr);

//     if (var_t_equal(found_var, POISON_VAR)) {
//         RAISE_TR_ERROR("var '%s' not initialized", var_name);
//         return;
//     }

//     snprintf(asm_payload->text_section, MAX_TEXT_SECTION_SZ,
//         "push [rbp + %d * %lu]; // access to '%s'\n",
//         found_var.loc_addr + 2, ASM_STACK_CELL_NMEMB, found_var.name);
// }




















#undef CHECK_NODE_TYPE
#undef RAISE_TR_ERROR