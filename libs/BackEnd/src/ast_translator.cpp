#include <assert.h>
#include <cstdio>
#include <string.h>

#include "AST_proc.h"
#include "ast_translator.h"
#include "AST_structs.h"
#include "diff_DSL.h"
#include "lang_logger.h"
#include "sections_processing.h"
#include "stack_frame_proc.h"
#include "FrontEnd.h"
#include "stack_funcs.h"
#include "string_funcs.h"
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
    dump_asm_payload_to_file("./testing_space/code.txt", &asm_payload);

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
        case AST_FUNC_INIT: gl_space->func_init = true; translate_function_definition(node, gl_space, asm_payload); gl_space->func_init = false; break;
        case AST_VAR_ID: translate_var_identifier(node, gl_space, asm_payload); break;
        case AST_SCOPE: translate_scope(node, gl_space, asm_payload); break;

        case AST_NUM_DOUBLE :
        case AST_NUM_INT64  :
        case AST_STR_LIT    :
            translate_constant(node, gl_space, asm_payload); break;

        // case NODE_OP: translate_op(node);
        //     break;
        // case NODE_TYPE: fprintf_red(stdout, "there is should be <NODE_TYPE> translation\n");
        //     break;
        // case NODE_ASSIGN: translate_assign(node);
        //     break;
        // case NODE_FUNC_ID: RAISE_TR_ERROR(
        //     "<NODE_FUNC_ID> should be processed in"
        //     "<translate_func_call/translate_function_definition>")
        //     break;
        // case NODE_CALL: translate_func_call(node);
        //     break;
        // case NODE_ELSE: RAISE_TR_ERROR(
        //     "<NODE_ELSE> should be processed in"
        //     "<translate_if>")
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
        // case NODE_EMPTY: RAISE_TR_ERROR("incorrect AST: <NODE_EMPTY>")
        //     break;
        // default: RAISE_TR_ERROR("incorrect AST: <UNKNOWN_NODE(%d)>", node->data.ast_node_type)
        //     break;

    }
}

void translate_constant(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    assert(
        node->data.ast_node_type == AST_NUM_DOUBLE  ||
        node->data.ast_node_type == AST_NUM_INT64   ||
        node->data.ast_node_type == AST_STR_LIT);

    switch (node->data.ast_node_type) {
        case AST_NUM_INT64:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "push   %ld;// push int64 constant\n",
            node->data.value.int64_val)
            return;
        case AST_NUM_DOUBLE:
            generate_mangled_name(bufer, BUFSIZ, "double_constant_", DEFAULT_MANGLING_SUFFIX_SZ);
            add_global_variable_record_to_data_section(asm_payload, bufer, DOUBLE_DATA_TYPE, node->data.value);

            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "movdqu xmm1, [%s];//                     \n"
                "sub    rsp, 16   ;// push double constant\n"
                "movdqu [rsp] xmm1;//                     \n",
                bufer)
            return;
        case AST_STR_LIT:
            generate_mangled_name(bufer, BUFSIZ, "str_literal_", DEFAULT_MANGLING_SUFFIX_SZ);
            add_global_variable_record_to_data_section(asm_payload, bufer, STRING_DATA_TYPE, node->data.value);

            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "lea    rbx, %s;//                     \n"
                "push   rbx    ;// push double constant\n",
                bufer)
            return;
        default:
            ast_node_type_write(bufer, BUFSIZ, node->data.ast_node_type);
            RAISE_TR_ERROR("error : `%s` isn't constant type\n", bufer)
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

void add_global_variable_record_to_data_section(asm_payload_t *asm_payload, char *var_name, const data_types var_data_type, const multi_val_t var_value) {
    assert(asm_payload);

    symbol_t global_var_sym = {};
    global_var_sym.sym_name = var_name;
    global_var_sym.sym_bind = LOCAL_OBJ_SYMBOL;
    global_var_sym.sym_section = DATA_SECTION;
    // WARNING------------------------------------
    global_var_sym.section_offset = -1; // FIXME:
    // WARNING------------------------------------
    global_var_sym.other_info = get_data_type_nmemb(var_data_type);

    add_symbol_to_name_table(&asm_payload->symbol_table, global_var_sym);



    switch (var_data_type) {
        case INT64_DATA_TYPE    :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\tdq %s: %ld\n", var_name, var_value.int64_val) return;
        case DOUBLE_DATA_TYPE   :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\tdq %s: %lf\n", var_name, var_value.double_val) return;
        case STRING_DATA_TYPE   :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\tdb %s: '%s'\n", var_name, var_value.sval) return;

        case NONE_TYPE: RAISE_TRANSLATOR_ERROR("var_data_type: `NONE_TYPE`, expected: INT64_DATA_TYPE|DOUBLE_DATA_TYPE|STRING_PTR|STRING_LITERAL");
    }
}

void translate_var_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_VAR_INIT);

    bool with_assignment                = (node->right->data.ast_node_type == AST_ASSIGN);
    bool global_state                   = (gl_space->cur_scope_deep == 0);
    lexer_token_t initialization_type   = (lexer_token_t) node->left->data.value.int64_val; assert(initialization_type == TOKEN_INT64_KEYWORD || initialization_type == TOKEN_DOUBLE_KEYWORD || initialization_type == TOKEN_STRING_KEYWORD);

    ast_tree_elem_t *var_ast_node       = with_assignment? node->right->left : node->right;
    char            *var_name           = var_ast_node->data.value.sval;
    int64_t          var_id             = var_ast_node->data.value.int64_val;

    multi_val_t      var_value          = with_assignment? node->right->right->data.value : EMPTY_MULTI_VAL;

    data_types       var_data_type      = convert_lexer_token_data_type(initialization_type);
    data_types_nmemb var_data_nmemb     = get_data_type_nmemb(var_data_type);

    if (global_state) {
        add_global_variable_record_to_data_section(asm_payload, var_name, var_data_type, var_value);
    } else {
        if (with_assignment) translate_node_to_asm_code(node->right->right, gl_space, asm_payload);

        var_t var_info = {};
        var_info.var_data_type      = var_data_type;
        var_info.var_data_nmemb     = var_data_nmemb;
        var_info.name_id            = var_id;
        var_info.name               = var_name;
        var_info.deep               = gl_space->cur_scope_deep;

        switch (var_data_type) {
            case INT64_DATA_TYPE:
                if (with_assignment) {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "mov    rbx, [rsp];// get access to stack last value (for assignment)\n"
                        "add    rsp, 8    ;// remove last value stack value                  \n"
                        "push   rbx       ;// save new local variable `%s` on stack          \n",
                    var_name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                } else {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 8    ;// reserve satck space for new local variable `%s`     \n",
                    var_info.name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                }

            case DOUBLE_DATA_TYPE:
            if (with_assignment) {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "movdqu xmm1, [rsp];// get access to stack last value for assignment    \n"
                        "add    rsp, 16    ;// remove last value from stack                     \n"
                        "sub    rsp, 16    ;// reserve stack space for new local variable `%s`  \n"
                        "movdqu [rsp], xmm1;//  save new local variable on stack                \n",
                    var_name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                } else {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 16    ;// reserve stack space for new local variable `%s`  \n",
                    var_name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                }

            case STRING_DATA_TYPE: // Для указателя на строковый литерал создаем строковый литерал в секции .data
                if (with_assignment) {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "lea    rbx, %s;// get access to global string literal (for assignment)\n"
                        "push   rbx    ;// save new local variable `%s` on stack               \n",
                    bufer, var_name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                } else {
                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 8    ;// reserve stack space for new local variable `%s`  \n",
                    var_name); add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr); return;
                }

            case NONE_TYPE:
                debug("error local variable type : NONE_TYPE!!\n"); abort();
        }
    }
}

void translate_function_definition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    CHECK_AST_NODE_TYPE(node, AST_FUNC_INIT);

    size_t argc = 0;

    lexer_token_t function_return_type = (lexer_token_t) node->left->data.value.int64_val;
    CHECK_AST_NODE_TYPE(node->left, AST_TYPE);


    symbol_t func_symbol = {};
    func_symbol.sym_name = node->right->data.value.sval;
    func_symbol.sym_type = FUNCTION_SYMBOL;
    // WARNING!!!!---------------------------------------------------
    func_symbol.section_offset = asm_payload->text_section_offset; // FIXME:!!!!
    // WARNING!!!!---------------------------------------------------
    func_symbol.sym_bind = GLOBAL_OBJ_SYMBOL;
    func_symbol.sym_section = TEXT_SECTION;
    func_symbol.other_info = 0; // количество байт, зарезервированных под инициализацию аргументов

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#=========Function========#\n"
        "%s:\n"
        ";#=======Input=Action======#\n"
        "push  rbp      \n"
        "mov   rbp, rsp \n"
        ";#=======End=Action========#\n",
        func_symbol.sym_name);

    ast_tree_elem_t *func_id_node = node->right;
    CHECK_NODE_TYPE(func_id_node, AST_FUNC_ID)

    if (func_id_node->left) func_symbol.other_info = (int64_t) translate_func_args_init(func_id_node->left, gl_space, asm_payload);
    add_symbol_to_name_table(&asm_payload->symbol_table, func_symbol);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#========Func=Body========#\n");

    translate_node_to_asm_code(func_id_node->right, gl_space, asm_payload); // func_body

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        ";#========End=Body=========#\n");

    size_t return_num = count_node_type_in_subtreeas(node->right, AST_RETURN);
    bool void_func_state = function_return_type == TOKEN_VOID;

    if (return_num == 0 && !void_func_state) {
        RAISE_TR_ERROR("non void function '%s' hasn't <return>", func_symbol.sym_name);
        return;
    }

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#=======Leave=Action======#\n"
        "mov    rsp, rbp\n"
        "pop    rbp     \n"
        "ret            \n"
        "%s_end:        \n"
        ";#=======End=Function======#\n",
        func_symbol.sym_name);

    var_stack_restore_old_frame(&gl_space->var_stack, gl_space->cur_frame_ptr); // call stack loc vars clearing + restore old_frame
}

size_t translate_func_args_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    assert(node->right);
    CHECK_NODE_TYPE(node, AST_COMMA);

    static size_t args_nmemb = 0;

    ast_tree_elem_t *var_init_node = node->right; // var_init
    CHECK_NODE_TYPE(var_init_node, AST_VAR_INIT);
    CHECK_NODE_TYPE(var_init_node->left, AST_TYPE);

    if (node->left) translate_func_args_init(node->left, gl_space, asm_payload);

    var_t var_info = {};
    var_info.var_data_type      = convert_lexer_token_data_type((lexer_token_t) var_init_node->left->data.value.int64_val);
    var_info.var_data_nmemb     = get_data_type_nmemb(var_info.var_data_type);
    var_info.name_id            = var_init_node->right->data.value.int64_val;
    var_info.deep               = gl_space->cur_scope_deep + 1; // + 1 т.к. переменная инициализируется уже внутри скойпа функции
    var_info.name               = var_init_node->right->data.value.sval;
    var_info.stack_frame_idx    = add_var_into_frame(var_info, &gl_space->var_stack, gl_space->cur_frame_ptr);

    args_nmemb += var_info.var_data_nmemb;

    if (!node->left) return args_nmemb;
    return 0;
}

void translate_var_identifier(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_VAR_ID);

    char *var_name = node->data.value.sval;             // строка хранится в string_storage, определенном в main.cpp
    int var_name_unic_id = node->data.value.int64_val;  // идентификатор стэкового фрэйма

    // На случай, если переменная глобальная:




    var_t local_var = get_var_from_frame(var_name_unic_id, &gl_space->var_stack, gl_space->cur_frame_ptr);
    bool local_var_defined = !var_t_equal(local_var, POISON_VAR_T);
    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!!
    symbol_t global_var_sym = get_global_variable_sym_from_name_table(&asm_payload->symbol_table, var_name); // FIXME:
    bool global_var_sym_defined = !symbol_t_equal(global_var_sym, POISON_SYMBOL);

    // WARNING. --------------------------------------------------------------------------------------------

    if (!local_var_defined && !global_var_sym_defined) {
        RAISE_TR_ERROR("var '%s' not initialized", var_name);
        return;
    }

    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!! FIXME:
    if (local_var_defined) {
        int var_stack_frame_offset = get_stack_frame_var_offset(&gl_space->var_stack, local_var.stack_frame_idx);

        switch (local_var.var_data_type) {
            case INT64_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "mov    rbx, [rbp + %d]; // access to local int64 '%s'  \n"
                    "push   rbx                                             \n",
                    var_stack_frame_offset + 16, local_var.name);
                return;

            case DOUBLE_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [rbp + %d]                            \n"
                    "sub    rsp, 16                                     \n"
                    "movdqu [rsp], xmm1 // access to local double '%s'  \n",
                    var_stack_frame_offset + 16, local_var.name
                )
                return;

            case STRING_DATA_TYPE: RAISE_TR_ERROR("error STRING_DATA_TYPE\n");
            case NONE_TYPE: RAISE_TR_ERROR("error NONE_TYPE\n");
        }
    }

    assert(global_var_sym_defined);
    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!! FIXME:
    if (global_var_sym_defined) {
        switch (local_var.var_data_type) {
            case INT64_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "mov    rbx, [%s]; // access to global int64 '%s'   \n"
                    "push   rbx                                         \n",
                    global_var_sym.sym_name, global_var_sym.sym_name);
                return;

            case DOUBLE_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [%s]                                  \n"
                    "sub    rsp, 16                                     \n"
                    "movdqu [rsp], xmm1 // access to global double '%s' \n",
                     global_var_sym.sym_name, global_var_sym.sym_name
                )
                return;

            case STRING_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "lea    rbx, %s; // access to global string '%s'    \n"
                    "push   rbx                                         \n",
                    global_var_sym.sym_name, global_var_sym.sym_name);
                return;

            case NONE_TYPE: RAISE_TR_ERROR("error NONE_TYPE\n");
        }
    }
}

void translate_scope(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);

    CHECK_NODE_TYPE(node, AST_SCOPE);

    gl_space->cur_scope_deep++;
    translate_node_to_asm_code(node->left, gl_space, asm_payload);
    gl_space->cur_scope_deep--;

    var_stack_remove_local_variables(&gl_space->var_stack, gl_space->cur_scope_deep);
}














#undef CHECK_NODE_TYPE
#undef RAISE_TR_ERROR