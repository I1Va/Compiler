#include <assert.h>
#include <cstdio>
#include <string.h>

#include "AST_proc.h"
#include "ast_translator.h"
#include "AST_structs.h"
#include "backend_structs.h"
#include "diff_DSL.h"
#include "sections_processing.h"
#include "asm_gl_space_proc.h"
#include "stack_funcs.h"
#include "string_funcs.h"
#include "backend_utils.h"

#include "backend_utils.h"

#define CHECK_NODE_TYPE(node, exp_type)                                         \
    if (node->data.ast_node_type != exp_type) {                                 \
        get_ast_node_descr(GLOBAL_BUFER, BUFSIZ, node->data.ast_node_type);     \
        RAISE_TR_ERROR("invalid_node: {%s}, expected: "#exp_type, GLOBAL_BUFER) \
    }

#define RAISE_TR_ERROR(str_, ...) {fprintf_red(stderr, "{%s} [%s: %d]: translator_error{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();}


static char GLOBAL_BUFER[BUFSIZ] = {};

void translator_global_space_init(asm_glob_space *gl_space, str_storage_t **storage) {
    assert(gl_space);

    gl_space->str_storage    = storage;
    gl_space->cur_scope_deep = 0;
    gl_space->while_counter  = 0;
    gl_space->if_counter     = 0;
    gl_space->func_init      = false;
    gl_space->void_func      = false;
    gl_space->cond_stack     = {};
    gl_space->var_stack      = {};

    STACK_INIT(&gl_space->cond_stack, 0, sizeof(int), NULL, NULL);
    STACK_INIT(&gl_space->var_stack, 0, sizeof(var_t), NULL, NULL);
    STACK_INIT(&gl_space->cpu_stack, 0, sizeof(cpu_stack_elem_t), NULL, NULL);
}

void translator_global_space_clear(asm_glob_space *gl_space) {
    assert(gl_space);

    stack_destroy(&gl_space->var_stack);
    stack_destroy(&gl_space->cond_stack);
    stack_destroy(&gl_space->cpu_stack);
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

bool prepare_standart_lib(asm_payload_t *lib_payload, const char std_lib_dir_path[]) {
    assert(lib_payload);
    assert(std_lib_dir_path);

    char data_section_path[BUFSIZ] = {};
    snprintf(data_section_path, BUFSIZ, "%s/.data", std_lib_dir_path);

    char text_section_path[BUFSIZ] = {};
    snprintf(text_section_path, BUFSIZ, "%s/.text", std_lib_dir_path);

    if (!load_std_lib(data_section_path, text_section_path, lib_payload)) {
        debug("failed to load_std_lib")
        return false;
    }

    return true;
}

void link_main_prog_with_lib(asm_payload_t *lib_payload, asm_payload_t *main_payload, asm_payload_t *outfile_payload) {
    assert(lib_payload);
    assert(main_payload);

    MAKE_RECORD_IN_DATA_SECTION(outfile_payload, "%s\n", lib_payload->data_section);
    MAKE_RECORD_IN_DATA_SECTION(outfile_payload, "%s", main_payload->data_section);
    MAKE_RECORD_IN_TEXT_SECTION(outfile_payload, "%s\n", lib_payload->text_section);
    MAKE_RECORD_IN_TEXT_SECTION(outfile_payload, "%s", main_payload->text_section);
}

void prepare_main_program(asm_payload_t *main_payload, ast_tree_t *tree, str_storage_t **storage) {
    assert(main_payload);
    assert(storage);
    assert(tree);


    asm_glob_space gl_space = {}; translator_global_space_init(&gl_space, storage);

    main_payload->data_section_offset += snprintf(main_payload->data_section + main_payload->data_section_offset,
                                                MAX_DATA_SECTION_SZ, "\tglobal main\n");


    translate_node_to_asm_code(tree->root, &gl_space, main_payload);
    translator_global_space_clear(&gl_space);
}

void translate_ast_to_asm_code(ast_tree_t *tree, str_storage_t **storage, const char outfile_path[], const char std_lib_dir[]) {
    assert(tree);
    assert(storage);
    assert(outfile_path);

    asm_payload_t lib_payload       = {}; prepare_standart_lib(&lib_payload, std_lib_dir);
    asm_payload_t main_payload      = {}; prepare_main_program(&main_payload, tree, storage);
    asm_payload_t outfile_payload   = {};

    link_main_prog_with_lib(&lib_payload, &main_payload, &outfile_payload);

    dump_asm_payload_to_file(outfile_path, &outfile_payload);
}

void translate_node_to_asm_code(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);

    switch (node->data.ast_node_type) {
        case AST_SEMICOLON: translate_semicolon(node, gl_space, asm_payload); break;
        case AST_VAR_INIT: translate_var_init(node, gl_space, asm_payload); break;

        case AST_FUNC_INIT:
            gl_space->func_init = true;
            translate_function_definition(node, gl_space, asm_payload);
            gl_space->func_init = false;
            break;

        case AST_VAR_ID: translate_var_identifier(node, gl_space, asm_payload); break;
        case AST_SCOPE: translate_scope(node, gl_space, asm_payload); break;

        case AST_NUM_DOUBLE :
        case AST_NUM_INT64  :
        case AST_STR_LIT    :
            translate_constant(node, gl_space, asm_payload); break;

        case AST_RETURN: translate_return_node(node, gl_space, asm_payload); break;
        case AST_CALL: translate_func_call(node, gl_space, asm_payload); break;
        case AST_OPERATION: translate_operation(node, gl_space, asm_payload); break;
        case AST_IF: translate_if(node, gl_space, asm_payload); break;
        case AST_WHILE: translate_while(node, gl_space, asm_payload); break;
        case AST_ASSIGN: translate_assign(node, gl_space, asm_payload); break;

        case AST_FUNC_ID: RAISE_TR_ERROR(
            "<NODE_FUNC_ID> should be processed in"
            "<translate_func_call/translate_function_definition>")
            break;

        case AST_ELSE: RAISE_TR_ERROR(
            "<NODE_ELSE> should be processed in"
            "<translate_if>")
            break;

        case AST_COMMA: RAISE_TR_ERROR(
            "<AST_COMMA> should be processed in"
            "<translate_func_call>/<translate_func_args_init>")
            break;

        case AST_TYPE: RAISE_TR_ERROR("incorrect AST: <AST_TYPE>"); break;
        default:
            get_ast_node_descr(GLOBAL_BUFER, BUFSIZ, node->data.ast_node_type);
            RAISE_TR_ERROR("incorrect AST: <UNKNOWN_NODE(%s)>", GLOBAL_BUFER); break;
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

    char *mangled_name = NULL;

    switch (node->data.ast_node_type) {
        case AST_NUM_INT64:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "push   %ld;// push int64 constant\n",
            node->data.value.int64_val)
            cpu_stack_push_constant(&gl_space->cpu_stack, INT64_DATA_TYPE, node->data.value);
            return;

        case AST_NUM_DOUBLE:
            mangled_name = get_new_str_ptr(gl_space->str_storage, MAX_SYMBOL_NAME_SZ);
            generate_mangled_name(mangled_name, MAX_SYMBOL_NAME_SZ, "double_constant_", DEFAULT_MANGLING_SUFFIX_SZ);
            add_global_variable_record_to_data_section(asm_payload, mangled_name, DOUBLE_DATA_TYPE, node->data.value);

            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "movdqu xmm1, [%s];//                     \n"
                "sub    rsp, 16   ;// push double constant\n"
                "movdqu [rsp], xmm1;//                     \n",
                mangled_name)
            cpu_stack_push_constant(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, node->data.value);
            return;

        case AST_STR_LIT:
            mangled_name = get_new_str_ptr(gl_space->str_storage, MAX_SYMBOL_NAME_SZ);
            generate_mangled_name(mangled_name, MAX_SYMBOL_NAME_SZ, "string_literal_", DEFAULT_MANGLING_SUFFIX_SZ);
            add_global_variable_record_to_data_section(asm_payload, mangled_name, STRING_DATA_TYPE, node->data.value);

            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "lea    rbx, [%s];//                          \n"
                "push   rbx      ;// push str_lit_ptr constant\n",
                mangled_name)

            cpu_stack_push_constant(&gl_space->cpu_stack, STRING_DATA_TYPE, node->data.value);
            return;

        default:
            get_ast_node_descr(GLOBAL_BUFER, BUFSIZ, node->data.ast_node_type);
            RAISE_TR_ERROR("error : `%s` isn't constant type\n", GLOBAL_BUFER)
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
    global_var_sym.var_sym_info.var_data_type = var_data_type;

    add_symbol_to_name_table(&asm_payload->symbol_table, global_var_sym);

    switch (var_data_type) {
        case INT64_DATA_TYPE    :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\t%s: dq %ld\n", var_name, var_value.int64_val) return;
        case DOUBLE_DATA_TYPE   :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\t%s: dq %lf\n", var_name, var_value.double_val) return;
        case STRING_DATA_TYPE   :  MAKE_RECORD_IN_DATA_SECTION(asm_payload, "\t%s: db '%s'\n", var_name, var_value.sval) return;

        case VOID_DATA_TYPE: RAISE_TRANSLATOR_ERROR("var_data_type: `VOID_DATA_TYPE`, expected: INT64_DATA_TYPE|DOUBLE_DATA_TYPE|STRING_PTR|STRING_LITERAL");
        case NONE_DATA_TYPE: RAISE_TRANSLATOR_ERROR("var_data_type: `NONE_DATA_TYPE`, expected: INT64_DATA_TYPE|DOUBLE_DATA_TYPE|STRING_PTR|STRING_LITERAL");
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
        var_info.var_data_type          = var_data_type;
        var_info.var_data_nmemb         = var_data_nmemb;
        var_info.name_id                = var_id;
        var_info.name                   = var_name;
        var_info.deep                   = gl_space->cur_scope_deep;
        var_info.var_type               = VAR_TYPE_LOCAL_VAR;
        var_info.base_pointer_offset    = add_local_var_into_frame(var_info, gl_space);

        switch (var_data_type) {
            case INT64_DATA_TYPE:
                if (with_assignment) {
                    get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);

                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        ";+ save new %s int64 variable `%s` on stack [rbp + %d]\n",
                    GLOBAL_BUFER, var_name, var_info.base_pointer_offset);
                    return;

                } else {
                    get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);

                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 8    ;// reserve stack space for new %s int64 variable `%s`\n",
                    GLOBAL_BUFER, var_info.name);
                    return;
                }

            case DOUBLE_DATA_TYPE:
            if (with_assignment) {
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    ";+ save new %s double variable `%s` on stack [rbp + %d]\n",
                GLOBAL_BUFER, var_name, var_info.base_pointer_offset);
                return;

                } else {
                    get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);

                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 16    ;// reserve stack space for new %s double variable `%s`\n",
                    GLOBAL_BUFER, var_name);
                    return;
                }

            case STRING_DATA_TYPE: // Для указателя на строковый литерал создаем строковый литерал в секции .data
                if (with_assignment) {
                    get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);

                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        ";// save new %s str_lit variable `%s` on stack          \n",
                    GLOBAL_BUFER, var_name);
                    return;
                } else {
                    get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, var_info.var_type);

                    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                        "sub    rsp, 8    ;// reserve stack space for new %s str_lit variable `%s`  \n",
                    GLOBAL_BUFER, var_name);
                    return;
                }
            case VOID_DATA_TYPE:
                RAISE_TRANSLATOR_ERROR("VOID_DATA_TYPE variable init")
            case NONE_DATA_TYPE:
                RAISE_TRANSLATOR_ERROR("NONE_DATA_TYPE variable init")
        }
    }
}

void translate_function_definition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    CHECK_AST_NODE_TYPE(node, AST_FUNC_INIT);


    lexer_token_t function_return_type = (lexer_token_t) node->left->data.value.int64_val;
    CHECK_AST_NODE_TYPE(node->left, AST_TYPE);
    data_types function_return_data_type = convert_lexer_token_data_type((lexer_token_t) node->left->data.value.int64_val);

    if (function_return_data_type == VOID_DATA_TYPE) gl_space->void_func = true;

    ast_tree_elem_t *function_identifier_node = node->right; CHECK_NODE_TYPE(function_identifier_node, AST_FUNC_ID)

    symbol_t func_symbol = {};
    func_symbol.sym_name = function_identifier_node->data.value.sval;
    func_symbol.sym_type = FUNCTION_SYMBOL;
    // WARNING!!!!---------------------------------------------------
    func_symbol.section_offset = asm_payload->text_section_offset; // FIXME:!!!!
    // WARNING!!!!---------------------------------------------------
    func_symbol.sym_bind = GLOBAL_OBJ_SYMBOL;
    func_symbol.sym_section = TEXT_SECTION;

    func_symbol.func_sym_info.args_cnt = count_node_type_in_subtrees(function_identifier_node->left, AST_VAR_INIT);
    func_symbol.func_sym_info.return_data_type = function_return_data_type;



    gl_space->cur_func_name = func_symbol.sym_name;


    ast_tree_elem_t *func_id_node = node->right;
    CHECK_NODE_TYPE(func_id_node, AST_FUNC_ID)

    if (func_id_node->left) {
        func_symbol.func_sym_info.args_summary_nmemb =
            translate_func_args_init(func_id_node->left, gl_space, asm_payload, FIRST_ARG_TRUE);
    }


    ast_tree_elem_t *scope_node = func_id_node->right;
    CHECK_NODE_TYPE(scope_node, AST_SCOPE)
    if (!scope_node->left) { // extern function
        func_symbol.sym_type = EXTERN_FUNCTION_SYMBOL;
        add_symbol_to_name_table(&asm_payload->symbol_table, func_symbol);
        MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
            "\n;=====extern_lib_fun========\n"
            "; %s                          \n"
            ";===========================\n\n",
            func_symbol.sym_name)

        gl_space->cur_func_name = NULL;
        return;
    }


    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#=========Function========#\n"
        "%s:\n"
        ";#=======Input=Action======#  \n"
        "push  rbp                     \n"
        "mov   rbp, rsp                \n"
        ";#=======End=Action========#  \n",
        func_symbol.sym_name);
    cpu_stack_push_base_pointer(&gl_space->cpu_stack);

    add_symbol_to_name_table(&asm_payload->symbol_table, func_symbol);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#========Func=Body========#\n");

    translate_node_to_asm_code(func_id_node->right, gl_space, asm_payload); // func_body

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        ";#========End=Body=========#\n");

    size_t return_num = count_node_type_in_subtrees(node->right, AST_RETURN);
    bool void_func_state = function_return_type == TOKEN_VOID;

    if (return_num == 0 && !void_func_state) {
        RAISE_TR_ERROR("non void function '%s' hasn't <return>", func_symbol.sym_name);
        return;
    }

    if (!var_stack_remove_local_variables(gl_space))
        RAISE_TRANSLATOR_ERROR("var_stack_remove_local_variables failed. func : `%s`", func_symbol.sym_name);


    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\n;#=======Leave=Action======#\n"
        "%s%s:                         \n"
        "mov    rsp, rbp               \n"
        "pop    rbp                    \n"
        "ret                           \n"
        ";#=======End=Function======#\n",
        func_symbol.sym_name, FUNCTION_LEAVE_SUFFIX);

    gl_space->cur_func_name = NULL;
}

size_t count_function_args_nmemb(ast_tree_elem_t *node) {
    assert(node);
    assert(node->right);
    CHECK_NODE_TYPE(node, AST_COMMA);

    ast_tree_elem_t *var_init_node = node->right; // var_init
    CHECK_NODE_TYPE(var_init_node, AST_VAR_INIT);
    CHECK_NODE_TYPE(var_init_node->left, AST_TYPE);



    data_types arg_data_type = convert_lexer_token_data_type((lexer_token_t) var_init_node->left->data.value.int64_val);
    size_t cur_arg_nmemb = get_data_type_nmemb(arg_data_type);

    if (node->left) return cur_arg_nmemb + count_function_args_nmemb(node->left);
    else return cur_arg_nmemb;

    return 0;
}

size_t translate_func_args_init(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, bool first_arg_in_rev_order) {
    if (!node) return 0;

    assert(gl_space);
    assert(asm_payload);
    assert(node->right);
    CHECK_NODE_TYPE(node, AST_COMMA);

    static size_t args_summary_nmemb = 0;
    if (first_arg_in_rev_order) {
        args_summary_nmemb = count_function_args_nmemb(node);
    }

    ast_tree_elem_t *var_init_node = node->right; // var_init
    CHECK_NODE_TYPE(var_init_node, AST_VAR_INIT);
    CHECK_NODE_TYPE(var_init_node->left, AST_TYPE);


    var_t farg_var = {};
    farg_var.var_data_type          = convert_lexer_token_data_type((lexer_token_t) var_init_node->left->data.value.int64_val);
    farg_var.var_data_nmemb         = get_data_type_nmemb(farg_var.var_data_type);
    farg_var.name_id                = var_init_node->right->data.value.int64_val;
    farg_var.deep                   = gl_space->cur_scope_deep + 1; // + 1 т.к. переменная инициализируется уже внутри скойпа функции
    farg_var.name                   = var_init_node->right->data.value.sval;
    farg_var.var_type               = VAR_TYPE_FUNCTION_ARG;


    if (first_arg_in_rev_order) {
        farg_var.base_pointer_offset = (int) args_summary_nmemb + FARGS_RBP_OFFSET - farg_var.var_data_nmemb;
        add_func_arg_into_frame(farg_var, gl_space, first_arg_in_rev_order);
    } else {
        farg_var.base_pointer_offset = UNKNOWN_BASE_POINTER_OFFSET;
        add_func_arg_into_frame(farg_var, gl_space, first_arg_in_rev_order);
    }
    if (node->left) {
        return farg_var.var_data_nmemb + translate_func_args_init(node->left, gl_space, asm_payload, false);
    } else {
        return farg_var.var_data_nmemb;
    }
}

void translate_var_identifier(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_VAR_ID);

    char *var_name = node->data.value.sval;             // строка хранится в string_storage, определенном в main.cpp
    int var_name_unic_id = node->data.value.int64_val;  // идентификатор стэкового фрэйма

    var_t local_var = get_var_from_frame(var_name_unic_id, &gl_space->var_stack, gl_space->cur_scope_deep);
    bool local_var_defined = !var_t_equal(local_var, POISON_VAR_T);
    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!!
    symbol_t global_var_sym = get_global_variable_sym_from_name_table(&asm_payload->symbol_table, var_name); // FIXME:
    bool global_var_sym_defined = !symbol_t_equal(global_var_sym, POISON_SYMBOL);
    // WARNING. --------------------------------------------------------------------------------------------

    if (!local_var_defined && !global_var_sym_defined) {
        dump_var_stack(stderr, &gl_space->var_stack);
        RAISE_TR_ERROR("cur_deep : {%d}, var '%s' not initialized", gl_space->cur_scope_deep, var_name);
        return;
    }

    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!! FIXME:
    if (local_var_defined) {
        switch (local_var.var_data_type) {
            case INT64_DATA_TYPE:
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, local_var.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "mov    rbx, [rbp + %d]; // access to %s int64 '%s'\n"
                    "push   rbx                                           \n",
                    local_var.base_pointer_offset, GLOBAL_BUFER, local_var.name);

                cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, local_var.name, CPU_STACK_VAR_VALUE);
                return;

            case DOUBLE_DATA_TYPE:
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, local_var.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [rbp + %d]                            \n"
                    "sub    rsp, 16                                     \n"
                    "movdqu [rsp], xmm1; // access to %s double '%s'  \n",
                    local_var.base_pointer_offset, GLOBAL_BUFER, local_var.name
                )
                cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, local_var.name, CPU_STACK_VAR_VALUE);
                return;

            case STRING_DATA_TYPE:
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, local_var.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "mov    rbx, [rbp %d]; // access to %s string literal ptr '%s'\n"
                    "push   rbx                                                   \n",
                    local_var.base_pointer_offset, GLOBAL_BUFER, local_var.name);
                cpu_stack_push_variable(&gl_space->cpu_stack, STRING_DATA_TYPE, local_var.name, CPU_STACK_VAR_VALUE);
                return;

            case NONE_DATA_TYPE: RAISE_TR_ERROR("error: NONE_DATA_TYPE var identifier\n");
            case VOID_DATA_TYPE: RAISE_TR_ERROR("error: VOID_DATA_TYPE var identifier\n");
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
                cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, global_var_sym.sym_name, CPU_STACK_VAR_VALUE);
                return;

            case DOUBLE_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [%s]                                  \n"
                    "sub    rsp, 16                                     \n"
                    "movdqu [rsp], xmm1;// access to global double '%s' \n",
                     global_var_sym.sym_name, global_var_sym.sym_name)
                cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, global_var_sym.sym_name, CPU_STACK_VAR_VALUE);
                return;

            case STRING_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "lea    rbx, %s; // access to global string '%s'    \n"
                    "push   rbx                                         \n",
                    global_var_sym.sym_name, global_var_sym.sym_name);
                cpu_stack_push_variable(&gl_space->cpu_stack, STRING_DATA_TYPE, global_var_sym.sym_name, CPU_STACK_VAR_VALUE);
                return;
            case VOID_DATA_TYPE: RAISE_TR_ERROR("error: VOID_DATA_TYPE var identifier \n");
            case NONE_DATA_TYPE: RAISE_TR_ERROR("error NONE_TYPE\n");
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

    if (!var_stack_remove_local_variables(gl_space)) RAISE_TRANSLATOR_ERROR("failed to remove locals in scope")
}

symbol_t *symbol_table_find(symbol_table_t *symbol_table, const char sym_name[]) {
    assert(symbol_table);
    assert(sym_name);


    for (size_t i = 0; i < symbol_table->table_sz; i++) {
        if (strncmp(symbol_table->data[i].sym_name, sym_name, MAX_SYMBOL_NAME_SZ) == 0) return &symbol_table->data[i];
    }

    return NULL;
}

void translate_funcs_call_args(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    if (!node) return;

    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_COMMA)


    translate_node_to_asm_code(node->right, gl_space, asm_payload);

    if (node->left) translate_funcs_call_args(node->left, gl_space, asm_payload);



}





size_t count_function_ast_args(ast_tree_elem_t *node) {
    if (!node) return 0;
    if (node->data.ast_node_type == AST_OPERATION) return 1;
    if (node->data.ast_node_type == AST_FUNC_ID) return 1;

    size_t count = (node->data.ast_node_type == AST_NUM_DOUBLE ||
                    node->data.ast_node_type == AST_NUM_INT64  ||
                    node->data.ast_node_type == AST_STR_LIT    ||
                    node->data.ast_node_type == AST_VAR_ID);


    if (node->left) count += count_function_ast_args(node->left);

    if (node->right) count += count_function_ast_args(node->right);

    return count;
}

void translate_func_call(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    CHECK_NODE_TYPE(node, AST_CALL);
    CHECK_NODE_TYPE(node->left, AST_FUNC_ID)

    char *function_name = node->left->data.value.sval; assert(function_name);
    symbol_t *function_symbol = symbol_table_find(&asm_payload->symbol_table, function_name);

    if (function_symbol == NULL) RAISE_TRANSLATOR_ERROR("symbol `%s` was not found in symbol_table", function_name)
    if (!(function_symbol->sym_type == FUNCTION_SYMBOL ||
          function_symbol->sym_type == EXTERN_FUNCTION_SYMBOL)) RAISE_TR_ERROR("symbol `%s` isn't FUNCTION_SYMBOL|EXTERN_FUNCTION_SYMBOL", function_name)

    size_t args_summary_nmemb   = function_symbol->func_sym_info.args_summary_nmemb;
    size_t argc                 = function_symbol->func_sym_info.args_cnt;
    data_types return_data_type = function_symbol->func_sym_info.return_data_type;


    translate_funcs_call_args(node->right, gl_space, asm_payload);

    size_t call_argc = count_function_ast_args(node->right);

    if (argc != call_argc) {
        RAISE_TR_ERROR("'%s' call error: expected %lu arguments, got '%lu'", function_symbol->sym_name, argc, call_argc);
        return;
    }

    cpu_stack_push_return_addr(&gl_space->cpu_stack);
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "call   %s      \n"
        "add    rsp, %lu\n",
        function_symbol->sym_name, args_summary_nmemb);
    cpu_stack_pop_return_addr(&gl_space->cpu_stack);


    switch (return_data_type) {
        case INT64_DATA_TYPE:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "push  rax;// push return value\n");
            cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?ret_val?", CPU_STACK_VAR_VALUE);
            break;

        case STRING_DATA_TYPE:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "push  rax;// push return value\n");
            cpu_stack_push_variable(&gl_space->cpu_stack, STRING_DATA_TYPE, "?ret_val?", CPU_STACK_VAR_VALUE);
            break;

        case DOUBLE_DATA_TYPE:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "sub    rsp, 16     ;// push return value\n"
                "movdqu [rsp], xmm0 ;//                  \n");
            cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, "?ret_val?", CPU_STACK_VAR_VALUE);
            break;

        case VOID_DATA_TYPE: break;

        case NONE_DATA_TYPE: RAISE_TRANSLATOR_ERROR("NONE_DATA_TYPE return data_type");
    }
}

void translate_return_node(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_RETURN);

    ast_tree_elem_t *return_val_node = node->left;

    cpu_stack_elem_t cpu_stack_last_elem = {};

    data_types return_data_type         = NONE_DATA_TYPE;
    char      *sym_name                 = NULL;
    char      *var_name                 = NULL;
    symbol_t  *global_return_sym_ptr    = NULL;
    var_t     local_ret_var             = POISON_VAR_T;



    if (return_val_node) {
        translate_node_to_asm_code(return_val_node, gl_space, asm_payload);
        switch (return_val_node->data.ast_node_type) {
            case AST_NUM_INT64: return_data_type = INT64_DATA_TYPE; break;
            case AST_NUM_DOUBLE: return_data_type = DOUBLE_DATA_TYPE; break;
            case AST_STR_LIT: return_data_type = STRING_DATA_TYPE; break;
            case AST_FUNC_ID:
                sym_name = return_val_node->data.value.sval; assert(sym_name);

                global_return_sym_ptr = symbol_table_find(&asm_payload->symbol_table, sym_name);
                if (!global_return_sym_ptr) RAISE_TRANSLATOR_ERROR("error : return_func_sym_ptr is nullptr. name: `%s`", sym_name)

                return_data_type = global_return_sym_ptr->func_sym_info.return_data_type;
                break;
            case AST_VAR_ID:
                var_name = return_val_node->data.value.sval; assert(var_name);

                // TRY GET LOCAL VAR
                local_ret_var = get_var_from_frame(return_val_node->data.value.int64_val, &gl_space->var_stack, gl_space->cur_scope_deep);
                if (!var_t_equal(local_ret_var, POISON_VAR_T)){
                    return_data_type = local_ret_var.var_data_type;
                    break;
                }
                // TRY GET GLOBAL VAR

                global_return_sym_ptr = symbol_table_find(&asm_payload->symbol_table, sym_name);
                if (global_return_sym_ptr) {
                    return_data_type = global_return_sym_ptr->var_sym_info.var_data_type;
                    break;
                }

                // ERROR. CAN'T DEFINE VAR DATA TYPE
                RAISE_TRANSLATOR_ERROR("error : can't define return data_type of `%s` var", var_name) break;

            case AST_CALL:
                sym_name = return_val_node->left->data.value.sval;
                global_return_sym_ptr = symbol_table_find(&asm_payload->symbol_table, sym_name);
                return_data_type = global_return_sym_ptr->func_sym_info.return_data_type;
                break;
            case AST_OPERATION:
                if (!check_cpu_stack_before_return(&gl_space->cpu_stack)) {
                    dump_cpu_stack(stderr, &gl_space->cpu_stack);
                    RAISE_TRANSLATOR_ERROR("cpu stack invalid state before return");
                }
                stack_get_elem(&gl_space->cpu_stack, &cpu_stack_last_elem, gl_space->cpu_stack.size - 1);
                return_data_type = cpu_stack_last_elem.data_type;
                break;

            default:
                get_ast_node_descr(GLOBAL_BUFER, BUFSIZ, return_val_node->data.ast_node_type);
                RAISE_TRANSLATOR_ERROR("error : return_val_node incorect type `%s`", GLOBAL_BUFER)
        }

        switch (return_data_type) {
            case INT64_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "pop    rax\n")
                cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
                break;

            case STRING_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "pop    rax\n")
                cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, STRING_DATA_TYPE);
                break;

            case DOUBLE_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm0, [rsp] \n"
                    "add    rsp, 16     \n")
                cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
                break;

            default:
                get_data_type_descr(GLOBAL_BUFER, BUFSIZ, return_data_type);
                RAISE_TRANSLATOR_ERROR("error : incorrect return_data_type `%s`", GLOBAL_BUFER)
        }
    }

    if (gl_space->cur_func_name == NULL) RAISE_TRANSLATOR_ERROR("return located out from function");

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "jmp   %s%s\n", gl_space->cur_func_name, FUNCTION_LEAVE_SUFFIX)
}

void translate_if(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    CHECK_NODE_TYPE(node, AST_IF);

    int save_counter = 0;
    ast_tree_elem_t *node_cond = node->left;
    node = node->right; // else_node

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        ";#=============If==========#\n"
        "jmp if_check_%d\n"
        "if_start_%d:    \n",
        gl_space->if_counter, gl_space->if_counter);


    stack_push(&gl_space->cond_stack, &gl_space->if_counter);
    stack_push(&gl_space->cond_stack, &gl_space->if_counter);
    gl_space->if_counter++;

    // if body
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, ";#=========If_body=========#\n")
    translate_node_to_asm_code(node->left, gl_space, asm_payload);
    stack_pop(&gl_space->cond_stack, &save_counter);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, ";#=========End_body========#\n")
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "jmp if_end_%d\n", save_counter)
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "else_start_%d:\n", save_counter)
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, ";#=======Else_body=========#\n")

    if (node->right) {
        translate_node_to_asm_code(node->right, gl_space, asm_payload);
        stack_pop(&gl_space->cond_stack, &save_counter);
    }

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, ";#=========End_body========#\n")
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "jmp if_end_%d\n", save_counter)
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "if_check_%d:\n\n", save_counter)

    //Condition
    translate_if_condition(node_cond, gl_space, asm_payload, save_counter);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "if_end_%d:\n"
        ";#=========End=IF=========#\n",
        save_counter)
}

void translate_if_condition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, int cur_cond_counter) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);

    translate_node_to_asm_code(node, gl_space, asm_payload); // stack push cond
    check_cpu_stack_before_condition(&gl_space->cpu_stack);
    data_types cond_arg_data_type = get_cpu_stack_last_var_data_type(&gl_space->cpu_stack);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, ";#========Condition========#\n")

    switch (cond_arg_data_type) {
        case INT64_DATA_TYPE:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "pop    rbx\n"
                "test   rbx, rbx\n")
            break;
        default:
            get_data_type_descr(GLOBAL_BUFER, BUFSIZ, cond_arg_data_type);
            RAISE_TRANSLATOR_ERROR("if doesn't support %s var cmp", GLOBAL_BUFER)
    }

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "je else_start_%d\n"
        "jmp if_start_%d\n"
        ";#======End=Condition=======#\n",
        cur_cond_counter, cur_cond_counter)
}

void translate_while(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);
    CHECK_NODE_TYPE(node, AST_WHILE);
    assert(node->left);
    assert(node->right);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        ";#===========While=========#\n"
        "jmp while_check_%d\n"
        "while_start_%d:\n\n",
        gl_space->while_counter, gl_space->while_counter);


    stack_push(&gl_space->cond_stack, &gl_space->while_counter);

    gl_space->while_counter++;

    if (node->right) {
        translate_node_to_asm_code(node->right, gl_space, asm_payload); //body
    }

    int save_counter = 0;
    stack_pop(&gl_space->cond_stack, &save_counter);     //pop index

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload, "\nwhile_check_%d:\n\n", save_counter)

    //Condition

    translate_while_condition(node->left, gl_space, asm_payload, save_counter);

    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "\nwhile_end_%d:\n"
        ";#=======End=While========#\n", save_counter)
}

void translate_while_condition(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload, int cur_counter) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);

    translate_node_to_asm_code(node, gl_space, asm_payload); // stack push cond

    check_cpu_stack_before_condition(&gl_space->cpu_stack);
    data_types cond_arg_data_type = get_cpu_stack_last_var_data_type(&gl_space->cpu_stack);

    switch (cond_arg_data_type) {
        case INT64_DATA_TYPE:
            MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                "pop    rbx\n"
                "test   rbx, rbx\n")
            break;
        default:
            get_data_type_descr(GLOBAL_BUFER, BUFSIZ, cond_arg_data_type);
            RAISE_TRANSLATOR_ERROR("while doesn't support %s var cmp", GLOBAL_BUFER)
    }
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        ";#========Condition========#\n"
        "jne while_start_%d\n",
        cur_counter);
}

void translate_assign(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(node->right);
    assert(node->left);
    CHECK_NODE_TYPE(node, AST_ASSIGN);

    char *var_name = node->left->data.value.sval;
    int var_name_id = node->left->data.value.int64_val;
    bool global_state = false;

    translate_node_to_asm_code(node->right, gl_space, asm_payload); // push right part of assign

    var_t local_var = get_var_from_frame(var_name_id, &gl_space->var_stack, gl_space->cur_scope_deep);
    bool local_var_defined = !var_t_equal(local_var, POISON_VAR_T);
    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!!
    symbol_t global_var_sym = get_global_variable_sym_from_name_table(&asm_payload->symbol_table, var_name); // FIXME:
    bool global_var_sym_defined = !symbol_t_equal(global_var_sym, POISON_SYMBOL);
    // WARNING. --------------------------------------------------------------------------------------------

    if (local_var_defined) {
        switch (local_var.var_data_type) {
            case INT64_DATA_TYPE:
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, local_var.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "pop    qword [rbp + %d]; // load val to %s int64 '%s'\n",
                    local_var.base_pointer_offset, GLOBAL_BUFER, local_var.name);

                cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, local_var.name, CPU_STACK_VAR_VALUE);
                return;

            case DOUBLE_DATA_TYPE:
                get_var_type_t_descr(GLOBAL_BUFER, BUFSIZ, local_var.var_type);
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [rsp]"
                    "add    rsp, 16"
                    "movdqu [rbp + %d], xmm1 ;// load value to %s double '%s'\n",
                    local_var.base_pointer_offset, GLOBAL_BUFER, local_var.name
                )
                return;

            case STRING_DATA_TYPE: RAISE_TRANSLATOR_ERROR("error : STRING_DATA_TYPE does not support assignment")
            case NONE_DATA_TYPE: RAISE_TR_ERROR("error: NONE_DATA_TYPE var identifier\n");
            case VOID_DATA_TYPE: RAISE_TR_ERROR("error: VOID_DATA_TYPE var identifier\n");
        }
    }

    assert(global_var_sym_defined);
    // WARNING. THERE IS SHOULD BE RELOCATION!!!!!! FIXME:
    if (global_var_sym_defined) {
        switch (local_var.var_data_type) {
            case INT64_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "pop    qword [%s]; // load val to global int64 '%s'\n",
                    global_var_sym.sym_name, global_var_sym.sym_name);

                cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, global_var_sym.sym_name, CPU_STACK_VAR_VALUE);
                return;

            case DOUBLE_DATA_TYPE:
                MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
                    "movdqu xmm1, [rsp]"
                    "add    rsp, 16"
                    "movdqu [%s], xmm1 // load val to global double '%s' \n",
                     global_var_sym.sym_name, global_var_sym.sym_name)
                cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, global_var_sym.sym_name, CPU_STACK_VAR_VALUE);
                return;

            case STRING_DATA_TYPE: RAISE_TRANSLATOR_ERROR("error : STRING_DATA_TYPE does not support assignment")
            case VOID_DATA_TYPE: RAISE_TR_ERROR("error: VOID_DATA_TYPE var identifier");
            case NONE_DATA_TYPE: RAISE_TR_ERROR("error NONE_TYPE");
        }
    }
}


void translate_op_add_int(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
            "pop    rbx     \n"
            "pop    rcx     \n"
            "add    rbx, rcx\n"
            "push   rbx     \n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_sub_int(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
            "pop    rbx     \n"
            "pop    rcx     \n"
            "sub    rbx, rcx\n"
            "push   rbx     \n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_mull_int(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
            "pop    rbx     \n"
            "pop    rcx     \n"
            "imul   rbx, rcx\n"
            "push   rbx     \n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_more_int(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "pop    rbx     \n"
        "pop    rcx     \n"
        "cmp    rbx, rcx\n"
        "setg   dl      \n"
        "movzx  rbx, dl \n"
        "push   rbx     \n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_less_int(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "pop    rbx     \n"
        "pop    rcx     \n"
        "cmp    rbx, rcx\n"
        "setl   dl      \n"
        "movzx  rbx, dl \n"
        "push   rbx     \n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, INT64_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, INT64_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_add_double(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
     MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "movdqu xmm1, [rsp]\n"
        "add    rsp, 16    \n"
        "movdqu xmm2, [rsp]\n"
        "add    rsp, 16    \n"
        "addpd  xmm1, xmm2 \n"
        "sub    rsp, 16    \n"
        "movdqu [rsp], xmm1\n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_sub_double(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "movdqu xmm1, [rsp]\n"
        "add    rsp, 16    \n"
        "movdqu xmm2, [rsp]\n"
        "add    rsp, 16    \n"
        "subpd  xmm1, xmm2 \n"
        "sub    rsp, 16    \n"
        "movdqu [rsp], xmm1\n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_mul_double(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
     MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "movdqu xmm1, [rsp]\n"
        "add    rsp, 16    \n"
        "movdqu xmm2, [rsp]\n"
        "add    rsp, 16    \n"
        "mulpd  xmm1, xmm2 \n"
        "sub    rsp, 16    \n"
        "movdqu [rsp], xmm1\n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}

void translate_op_div_double(asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    MAKE_RECORD_IN_TEXT_SECTION(asm_payload,
        "movdqu xmm1, [rsp]\n"
        "add    rsp, 16    \n"
        "movdqu xmm2, [rsp]\n"
        "add    rsp, 16    \n"
        "divsd  xmm1, xmm2 \n"
        "sub    rsp, 16    \n"
        "movdqu [rsp], xmm1\n")

    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_pop_value_for_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE);
    cpu_stack_push_variable(&gl_space->cpu_stack, DOUBLE_DATA_TYPE, "?op_var?", CPU_STACK_VAR_VALUE);
}


void translate_operation(ast_tree_elem_t *node, asm_glob_space *gl_space, asm_payload_t *asm_payload) {
    assert(node);
    assert(gl_space);
    assert(asm_payload);


    if (node->data.ast_node_type == AST_NUM_INT64   ||
        node->data.ast_node_type == AST_NUM_DOUBLE  ||
        node->data.ast_node_type == AST_STR_LIT) {
            translate_constant(node, gl_space, asm_payload); return;
    } else if (node->data.ast_node_type == AST_VAR_ID) {
        translate_var_identifier(node, gl_space, asm_payload); return;
    } else if (node->data.ast_node_type == AST_CALL) {
        translate_func_call(node, gl_space, asm_payload);
        return;
    }

    CHECK_NODE_TYPE(node, AST_OPERATION);

    translate_operation(node->right, gl_space, asm_payload);
    translate_operation(node->left, gl_space, asm_payload);


    if (!check_prepared_for_operation_args(&gl_space->cpu_stack)) RAISE_TRANSLATOR_ERROR("failed to prepare args for operation")

    cpu_stack_elem_t arg_left = {};
    cpu_stack_elem_t arg_right = {};
    stack_get_elem(&gl_space->cpu_stack, &arg_left, gl_space->cpu_stack.size - 1);
    stack_get_elem(&gl_space->cpu_stack, &arg_right, gl_space->cpu_stack.size - 2);

    data_types args_data_type = arg_left.data_type; assert(arg_left.data_type == arg_right.data_type);

    if (args_data_type == INT64_DATA_TYPE) {
        switch ((lexer_token_t) node->data.value.int64_val) {
            case TOKEN_ADD:
                translate_op_add_int(gl_space, asm_payload);
                return;
            case TOKEN_SUB:
                translate_op_sub_int(gl_space, asm_payload);
                return;
            case TOKEN_MUL:
                translate_op_mull_int(gl_space, asm_payload);
                return;
            case TOKEN_MORE:
                translate_op_more_int(gl_space, asm_payload);
                return;
            case TOKEN_LESS:
                translate_op_less_int(gl_space, asm_payload);
                return;

            default: break;
        }
    }

    if (args_data_type == DOUBLE_DATA_TYPE) {
        switch ((lexer_token_t) node->data.value.int64_val) {
            case TOKEN_ADD:
                translate_op_add_double(gl_space, asm_payload);
                return;
            case TOKEN_SUB:
                translate_op_sub_double(gl_space, asm_payload);
                return;
            case TOKEN_MUL:
                translate_op_mul_double(gl_space, asm_payload);
                return;
            case TOKEN_DIV:
                translate_op_div_double(gl_space, asm_payload);
                return;

            default: break;
        }
    }


    int data_type_offset = 1 + get_token_descr(GLOBAL_BUFER, BUFSIZ, (lexer_token_t) node->data.value.int64_val);
    get_data_type_descr(GLOBAL_BUFER + data_type_offset, BUFSIZ, args_data_type);
    RAISE_TRANSLATOR_ERROR("operation `%s` doesn't support args data type of `%s`", GLOBAL_BUFER, GLOBAL_BUFER + data_type_offset)

//         // case AST_MUL: fprintf(asm_code_ptr, "mult;\n"); break;
//         // case AST_SUB: fprintf(asm_code_ptr, "sub;\n"); break;
//         // case AST_DIV: fprintf(asm_code_ptr, "div;\n"); break;
//         // case AST_LESS: fprintf(asm_code_ptr, "less;\n"); break;
//         // case AST_LESS_EQ: fprintf(asm_code_ptr, "lesseq;\n"); break;
//         // case AST_MORE: fprintf(asm_code_ptr, "more;\n"); break;
//         // case AST_MORE_EQ: fprintf(asm_code_ptr, "moreeq;\n"); break;
//         // case AST_EQ: fprintf(asm_code_ptr, "eq;\n"); break;
//         // default:
//         //     debug("translate_op UNKNOWN_OP(%d)", node->data.value.ival);
//         //     fprintf(asm_code_ptr, "UNKNOWN_OP(%d);\n", node->data.value.ival);
//         // break;
//     }
}


#undef CHECK_NODE_TYPE
#undef RAISE_TR_ERROR