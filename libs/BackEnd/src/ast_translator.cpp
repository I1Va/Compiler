    #include <assert.h>
    #include <cstdio>
    #include <string.h>

    #include "AST_proc.h"
    #include "ast_translator.h"
    #include "AST_structs.h"
    #include "diff_DSL.h"
    #include "lang_logger.h"
    #include "stack_frame_proc.h"
    #include "AST_io.h"
    #include "FrontEnd.h"
    #include "stack_funcs.h"
    #include "string_funcs.h"

    #include "elf.h"
    #include "translator_general.h"

    #define CHECK_NODE_TYPE(node, exp_type)                                                     \
        if (node->data.ast_node_type != exp_type) {                                                  \
            RAISE_TR_ERROR("invalid_node: {%d}, expected: "#exp_type, node->data.ast_node_type) \
        }

    #define RAISE_TR_ERROR(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();

    static char bufer[BUFSIZ] = {};

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

    void dump_asm_payload_to_file(const char path[], asm_payload_t *asm_payload) {
        assert(path);
        assert(asm_payload);

        FILE *file = fopen(path, "w");
        if (!file) {
            debug("file '%s' open failed\n", path);
            abort();
        }

        fprintf(file,
            "section .data\n%s\n"
            "section .text\n%s\n",
            asm_payload->data_section,
            asm_payload->text_section
        );
    }

    void translate_ast_to_asm_code(ast_tree_t *tree) {
        assert(tree);

        asm_glob_space gl_space = {}; translator_global_space_init(&gl_space);
        asm_payload_t asm_payload = {};

        translate_node_to_asm_code(tree->root, &gl_space, &asm_payload);
        dump_asm_payload_to_file("./testing_space/code.asm", &asm_payload);

        translator_global_space_clear(&gl_space);

        // fprintf(asm_code_ptr,
        //                     "call main:\n"
        //                     "hlt;\n");
    }


    void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
        assert(node);
        assert(gl_space);
        assert(asm_payload);

        switch (node->data.ast_node_type) {
            case AST_SEMICOLON: translate_semicolon(node, gl_space, asm_payload); break;
            case AST_VAR_INIT: translate_var_init(node, gl_space, asm_payload); break;
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
        CHECK_NODE_TYPE(node, AST_SEMICOLON);

        if (node->left) {
            translate_node_to_asm_code(node->left, gl_space, asm_payload);
        }
        if (node->right) {
            translate_node_to_asm_code(node->right, gl_space, asm_payload);
        }
    }

    void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, int init_type, const multi_val_t var_value) {
        assert(asm_payload);
        assert(var_name);

        switch (init_type) {
            case TOKEN_INT64_KEYWORD : asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %ld\n", var_name, var_value.int64_val); return;
            case TOKEN_DOUBLE_KEYWORD: asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdq %s: %lf\n", var_name, var_value.double_val); return;
            case TOKEN_STRING_KEYWORD: asm_payload->data_section_offset += snprintf(asm_payload->data_section + asm_payload->data_section_offset, MAX_DATA_SECTION_SZ, "\tdb %s: '%s'\n", var_name, var_value.sval); return;
            default: token_write(bufer, BUFSIZ, (lexer_token_t) init_type); RAISE_TR_ERROR("invalid_token: {%s}, expected: TOKEN_STRING_KEYWORD|TOKEN_DOUBLE_KEYWORD|TOKEN_INT64_KEYWORD", bufer);
        }
    }

    void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz) {
        assert(bufer);

        size_t prefix_sz = strnlen(prefix, buf_sz);
        snprintf(bufer, buf_sz, "%s", prefix);
        generate_random_string(bufer + prefix_sz, mangling_suf_sz);
    }

    void add_local_variable_record_to_text_section(asm_payload_t *asm_payload, char *var_name, int init_type, const multi_val_t var_value) {
        assert(asm_payload);
        assert(var_name);

        switch (init_type) {
            case TOKEN_INT64_KEYWORD:
                asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                    "push   %ld", var_value.int64_val); return;

            case TOKEN_DOUBLE_KEYWORD:
                asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                    "sub    rsp, 16"
                    "movdqu [rsp], %lf",
                    var_value.double_val); return;

            case TOKEN_STRING_KEYWORD:
                generate_mangled_name(bufer, BUFSIZ, "str_literal_", DEFAULT_MANGLING_SUFFIX_SZ);
                char *string_literal_global_name = bufer;

                asm_payload->text_section_offset += snprintf(asm_payload->text_section + asm_payload->text_section_offset, MAX_TEXT_SECTION_SZ,
                    "sub    rsp, 8"
                    "lea    [rsp], %s",
                    string_literal_global_name); return;
        }
    }


    void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
        assert(node);
        assert(gl_space);
        assert(asm_payload);
        CHECK_NODE_TYPE(node, AST_VAR_INIT);

        bool with_assignment            = (node->right->data.ast_node_type == AST_ASSIGN);
        bool global_state               = (gl_space->cur_scope_deep == 0);
        int  initialization_type        = node->left->data.value.int64_val; assert(initialization_type == TOKEN_INT64_KEYWORD || initialization_type == TOKEN_DOUBLE_KEYWORD || initialization_type == TOKEN_STRING_KEYWORD);

        ast_tree_elem_t *var_ast_node   = with_assignment? node->right->left : node->right;
        char            *var_name       = var_ast_node->data.value.sval;
        int              var_id         = var_ast_node->data.value.int64_val;

        multi_val_t      var_value      = with_assignment? node->right->right->data.value : EMPTY_MULTI_VAL;

        if (global_state) {
            add_global_variable_record_to_data_section(asm_payload, var_name, initialization_type, var_value);
        } else {
            var_t var_info = {};
            var_info.deep       = gl_space->cur_scope_deep;
            var_info.name       = var_name;
            var_info.name_id    = var_value.int64_val;
            var_info.loc_addr   = add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr);

            add_local_variable_record_to_text_section(asm_payload, var_name, initialization_type, var_value);
        }
    }




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