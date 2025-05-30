#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <sys/types.h>

#include "AST_proc.h"
#include "AST_structs.h"
#include "general.h"
#include "FrontEnd.h"
#include "lang_logger.h"
#include "string_funcs.h"
#include "lang_logger.h"
#include "lang_grammar.h"
#include "diff_DSL.h"

// GRAMMAR:
// #=================================================================================================================#
// get_syntax_analysis: get_statement_list_untill_eof <EOF>

// get_expression: get_logical_expression

// get_statement:
//     | get_function_initialization
//     | get_while
//     | get_cont_ret_break
//     | get_function_call
//     | get_assignment
//     | get_variable_initialization_with_assignment
//     | get_variable_initialization
//     | get_expression
//     | get_selection_statement

// get_return: <return> get_expression

// get_cont_ret_break:
//     | get_return
//     | <break>
//     | <continue>

// get_selection_statement: <if> <(> get_expression <)> get_scope {<else> get_scope}*

// get_logical_expression: get_additive_expression {<logical_op> get_additive_expression}*

// get_additive_expression: get_multiplicative_expression {<additive_op> get_multiplicative_expression}*

// get_multiplicative_expression: get_direct_declarator {<multiplicative_op> get_direct_declarator}*

// get_direct_declarator:
//     | get_additive_expression
//     | get_function_call
//     | get_primary_expression

// get_type: <type_word>

// get_variable_initialization: get_type get_variable

// get_variable_initialization_with_assignment: get_type get_assignment

// get_func_separated_init_args: {get_variable_initialization <,>}*

// get_statement_list_untill_eof: {get_statement <;>}*

// get_scope: <{> {get_statement <;>}* <}>

// get_function_initialization: get_type get_func_identificator <(> get_func_separated_init_args <)> get_scope

// get_function_call: get_func_identificator <(> {get_expression(data) <,>}* <)>

// get_assignment: get_variable <=> get_expression

// get_primary_expression:
//     | get_variable
//     | get_constant
//     | get_string_literal

// get_while: <while> <(> get_expression <)> get_scope

// get_constant: <NUM>

// get_string_literal: <STR_LIT>

// get_variable: <ID>

// get_func_identificator: <ID>
// #=================================================================================================================#

#define CATCH_PARSE_ERROR(data, grule, exit_com)           \
    if (data->parser_err.err_state) {                      \
        add_grule_to_parser_err(&data->parser_err, grule); \
        exit_com;                                          \
    }

#define STEP_OVER_TOKEN_WITH_CHECK(data, grule, token, exit_com)                            \
    if (data->lexem_list[data->lexem_list_idx].token_type != token) {                       \
        start_parser_err(data, data->lexem_list[data->lexem_list_idx], grule);              \
        exit_com;                                                                           \
    }                                                                                       \
    data->lexem_list_idx++;

ast_tree_elem_t *try_grule(parsing_block_t *data, ast_tree_elem_t *(*grule_func)(parsing_block_t *data)) {
    assert(data);
    assert(grule_func);

    size_t tp = data->lexem_list_idx;
    ast_tree_elem_t *node = NULL;

    node = grule_func(data);
    if (!data->parser_err.err_state) {
        return node;
    }
    clear_parser_err(&data->parser_err);
    data->lexem_list_idx = tp;
    return NULL;
}

void start_parser_err(parsing_block_t *data, lexem_t lexem, enum grammar_rule_num grule) {
    assert(data);
    if (data->parser_err.err_state) {
        check_parser_err(stdout, data);
        assert(!data->parser_err.err_state);
    }

    assert(data->parser_err.lex.token_type == TOKEN_EMPTY);
    assert(!data->parser_err.grule_list_size);

    data->parser_err.err_state = true;
    data->parser_err.lex = lexem;
    data->parser_err.grule_list[0] = grule;
    data->parser_err.grule_list_size++;
}

void add_grule_to_parser_err(parser_err_t *parser_err, enum grammar_rule_num grule) {
    assert(parser_err);
    assert(parser_err->err_state);

    parser_err->grule_list[parser_err->grule_list_size++] = grule;
}

void clear_parser_err(parser_err_t *parser_err) {
    assert(parser_err);
    assert(parser_err->err_state);

    parser_err->grule_list_size = 0;
    parser_err->grule_list[0] = EMPTY_GRULE;
    parser_err->err_state = false;
    parser_err->lex = {};
    parser_err->lex.token_type = TOKEN_EMPTY;
}

bool check_parser_err(FILE *stream, parsing_block_t *data) {
    assert(stream);
    assert(data);

    if (!data->parser_err.err_state) {
        return false;
    }

    fprintf_title(stream, "SYNTAX_ERROR", '-', STR_F_BORDER_SZ);

    fprintf(stream, "ERROR_LEXEM: ");
    lexem_dump(stream, data, data->parser_err.lex);
    printf("\n");

    fprintf(stream, "ERROR_PATH: \n");
    for (size_t i = 0; i < data->parser_err.grule_list_size; i++) {
        grule_dump(stream, data->parser_err.grule_list[i]);
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");

    fprintf_border(stream, '-', STR_F_BORDER_SZ, true);

    return true;
}

void dump_last_lex(parsing_block_t *data) {
    assert(data);

    fprintf_title(stdout, "DEBUG_LEX", '-', STR_F_BORDER_SZ);
    lexem_dump(stdout, data, data->lexem_list[data->lexem_list_idx]);
    printf("\n");
    fprintf_border(stdout, '-', STR_F_BORDER_SZ, true);
}

ast_tree_elem_t *get_syntax_analysis(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *global_statement_list = NULL;
    int empty = 0;

    empty = 1;
    global_statement_list = get_statement_list_untill_eof(data, &empty);
    CATCH_PARSE_ERROR(data, GET_SYNTAX_ANALYSIS, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SYNTAX_ANALYSIS, TOKEN_EOF, CLEAR_MEMORY(exit_mark))

    return global_statement_list;

    exit_mark:

    sub_tree_dtor(global_statement_list);
    return NULL;
}

ast_tree_elem_t *get_expression(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *node = get_logical_expression(data);
    CATCH_PARSE_ERROR(data, GET_EXPRESSION, return NULL);

    return node;
}

ast_tree_elem_t *get_statement(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *val = NULL;

    val = try_grule(data, get_function_initialization);
    if (val) { return val; }

    val = try_grule(data, get_while);
    if (val) { return val; }

    val = try_grule(data, get_cont_ret_break);
    if (val) {return val; }

    val = try_grule(data, get_function_call);
    if (val) { return val; }

    val = try_grule(data, get_assignment);
    if (val) { return val; }

    val = try_grule(data, get_variable_initialization_with_assignment);
    if (val) { return val; }

    val = try_grule(data, get_variable_initialization);
    if (val) { return val; }

    val = try_grule(data, get_expression);
    if (val) { return val; }

    val = get_selection_statement(data);
    CATCH_PARSE_ERROR(data, GET_STATEMENT, return NULL)

    return val;
}

ast_tree_elem_t *get_return(parsing_block_t *data) {
    assert(data);

    size_t *tp = &data->lexem_list_idx;
    lexem_t lexem = data->lexem_list[*tp];
    ast_tree_elem_t *return_node = NULL;
    STEP_OVER_TOKEN_WITH_CHECK(data, GET_RETURN, lexem.token_type, return NULL)

    return_node = try_grule(data, get_expression);

    return _RETURN(return_node);
}

ast_tree_elem_t *get_cont_ret_break(parsing_block_t *data) {
    assert(data);

    size_t *tp = &data->lexem_list_idx;
    lexem_t lexem = data->lexem_list[*tp];

    switch (lexem.token_type) {
        case TOKEN_RETURN  :          return get_return(data);
        case TOKEN_BREAK   : (*tp)++; return _BREAK();
        case TOKEN_CONTINUE: (*tp)++; return _CONTINUE();
        default: start_parser_err(data, lexem, GET_CONT_RET_BREAK); return NULL;
    }
}

ast_tree_elem_t *get_selection_statement(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *expr_node = NULL;
    ast_tree_elem_t *if_body_node = NULL;
    ast_tree_elem_t *else_body_node = NULL;
    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    int empty = 0;

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SELECTION_STATEMENT, TOKEN_IF, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SELECTION_STATEMENT, TOKEN_O_BRACE, CLEAR_MEMORY(exit_mark))

    expr_node = get_expression(data);
    CATCH_PARSE_ERROR(data, GET_SELECTION_STATEMENT, CLEAR_MEMORY(exit_mark));

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SELECTION_STATEMENT, TOKEN_C_BRACE, CLEAR_MEMORY(exit_mark))

    empty = 1;
    if_body_node = get_scope(data, &empty);
    CATCH_PARSE_ERROR(data, GET_SELECTION_STATEMENT, CLEAR_MEMORY(exit_mark));

    if (tl[*tp].token_type == TOKEN_ELSE) {
        STEP_OVER_TOKEN_WITH_CHECK(data, GET_SELECTION_STATEMENT, TOKEN_ELSE, CLEAR_MEMORY(exit_mark))

        empty = 1;
        else_body_node = get_scope(data, &empty);
        CATCH_PARSE_ERROR(data, GET_SELECTION_STATEMENT, CLEAR_MEMORY(exit_mark))
        if (empty) {
            start_parser_err(data, data->lexem_list[data->lexem_list_idx], GET_SELECTION_STATEMENT);
            CLEAR_MEMORY(exit_mark)
        }

    }

    return _IF(expr_node, _ELSE(if_body_node, else_body_node));

    exit_mark:

    sub_tree_dtor(expr_node);
    sub_tree_dtor(if_body_node);
    sub_tree_dtor(else_body_node);

    return NULL;
}

bool check_token_on_logical_class(const lexer_token_t token) {
    return
    (
    token == TOKEN_LESS    ||
    token == TOKEN_MORE    ||
    token == TOKEN_MORE_EQ ||
    token == TOKEN_LESS_EQ ||
    token == TOKEN_EQ
    );
}

ast_tree_elem_t *get_logical_expression(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);
    ast_tree_elem_t *node = NULL;
    ast_tree_elem_t *new_node = NULL;

    node = get_additive_expression(data);
    CATCH_PARSE_ERROR(data, GET_LOGICAL_EXPRESSION, CLEAR_MEMORY(exit_mark));

    while (check_token_on_logical_class(tl[*tp].token_type)) {
        int op_num = tl[*tp].token_type;
        (*tp)++;

        new_node = get_additive_expression(data);
        CATCH_PARSE_ERROR(data, GET_LOGICAL_EXPRESSION, CLEAR_MEMORY(exit_mark))

        node = _OP(op_num, node, new_node);
    }

    return node;

    exit_mark:

    sub_tree_dtor(node);
    sub_tree_dtor(new_node);

    return NULL;
}

bool check_token_on_additive_class(const lexer_token_t token) {
    return
    (
    token == TOKEN_ADD     ||
    token == TOKEN_SUB
    );
}

ast_tree_elem_t *get_additive_expression(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);
    ast_tree_elem_t *node = NULL;
    ast_tree_elem_t *new_node = NULL;

    node = get_multiplicative_expression(data);
    CATCH_PARSE_ERROR(data, GET_ADDITIVE_EXPRESSION, CLEAR_MEMORY(exit_mark))

    while (check_token_on_additive_class(tl[*tp].token_type)) {
        int op_num = tl[*tp].token_type;
        (*tp)++;

        new_node = get_multiplicative_expression(data);
        CATCH_PARSE_ERROR(data, GET_MULTIPLICATIVE_EXPRESSION, CLEAR_MEMORY(exit_mark))
        multi_val_t op_multi_val = {};
        op_multi_val.int64_val = op_num; // идентификатор операции
        node = _OP(op_num, node, new_node);
    }

    return node;

    exit_mark:

    sub_tree_dtor(node);
    sub_tree_dtor(new_node);

    return NULL;
}

bool check_token_on_multiplicative_class(const lexer_token_t token) {
    return
    (
    token == TOKEN_MUL     ||
    token == TOKEN_DIV
    );
}

ast_tree_elem_t *get_multiplicative_expression(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);
    ast_tree_elem_t *node = NULL;
    ast_tree_elem_t *new_node = NULL;

    node = get_direct_declarator(data);
    CATCH_PARSE_ERROR(data, GET_MULTIPLICATIVE_EXPRESSION, CLEAR_MEMORY(exit_mark))

    while (check_token_on_multiplicative_class(tl[*tp].token_type)) {
        int op_num = tl[*tp].token_type;
        (*tp)++;

        new_node = get_direct_declarator(data);
        CATCH_PARSE_ERROR(data, GET_MULTIPLICATIVE_EXPRESSION, CLEAR_MEMORY(exit_mark))

        node = _OP(op_num, node, new_node);
    }

    return node;

    exit_mark:

    sub_tree_dtor(node);
    sub_tree_dtor(new_node);

    return NULL;
}

bool check_token_on_primary_expr_class(const lexer_token_t token) {
    return
    (
    token == TOKEN_ID         ||
    token == TOKEN_NUM_INT64  ||
    token == TOKEN_NUM_DOUBLE ||
    token == TOKEN_STR_LIT
    );
}

ast_tree_elem_t *get_direct_declarator(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    if (tl[*tp].token_type == TOKEN_O_BRACE) {
        (*tp)++;

        ast_tree_elem_t *val = get_additive_expression(data);
        CATCH_PARSE_ERROR(data, GET_DIRECT_DECLARATOR, return NULL);

        STEP_OVER_TOKEN_WITH_CHECK(data, GET_DIRECT_DECLARATOR, TOKEN_C_BRACE, return NULL);

        return val;
    }

    if (tl[*tp].token_type == TOKEN_ID && tl[(*tp) + 1].token_type == TOKEN_O_BRACE) {
        ast_tree_elem_t *func_node = get_function_call(data);
        CATCH_PARSE_ERROR(data, GET_DIRECT_DECLARATOR, return NULL)

        return func_node;
    }

    if (check_token_on_primary_expr_class(tl[*tp].token_type)) {
        ast_tree_elem_t *prim_node = get_primary_expression(data);
        CATCH_PARSE_ERROR(data, GET_DIRECT_DECLARATOR, return NULL)

        return prim_node;
    }

    start_parser_err(data, tl[*tp], GET_DIRECT_DECLARATOR);
    return NULL;

}

bool check_token_on_type_class(const lexer_token_t token) {
    return
    (
    token == TOKEN_INT64_KEYWORD   ||
    token == TOKEN_STRING_KEYWORD  ||
    token == TOKEN_DOUBLE_KEYWORD
    );
}

ast_tree_elem_t *get_type(parsing_block_t *data) {
    assert(data);
    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);
    ast_tree_elem_t *type_node = NULL;
    lexer_token_t token = tl[*tp].token_type;

    if (!(check_token_on_type_class(token) || token == TOKEN_VOID)) {
        start_parser_err(data, tl[*tp], GET_TYPE);
        return NULL;
    }

    (*tp)++;

    type_node = _TYPE(token);

    return type_node;
}

ast_tree_elem_t *get_variable_initialization(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *type_node = NULL;
    ast_tree_elem_t *main_node = NULL;

    type_node = get_type(data);
    CATCH_PARSE_ERROR(data, GET_VARIABLE_INITIALIZATION, CLEAR_MEMORY(exit_mark))

    main_node = get_variable(data);
    CATCH_PARSE_ERROR(data, GET_VARIABLE_INITIALIZATION, CLEAR_MEMORY(exit_mark))

    return _VAR_INIT(type_node, main_node);

    exit_mark:

    sub_tree_dtor(type_node);
    sub_tree_dtor(main_node);

    return NULL;
}

ast_tree_elem_t *get_variable_initialization_with_assignment(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *type_node = NULL;
    ast_tree_elem_t *main_node = NULL;

    type_node = get_type(data);
    CATCH_PARSE_ERROR(data, GET_VARIABLE_INITIALIZATION_WITH_ASSIGNMENT, CLEAR_MEMORY(exit_mark))

    main_node = get_assignment(data);
    CATCH_PARSE_ERROR(data, GET_VARIABLE_INITIALIZATION_WITH_ASSIGNMENT, CLEAR_MEMORY(exit_mark))

    return _VAR_INIT(type_node, main_node);

    exit_mark:

    sub_tree_dtor(type_node);
    sub_tree_dtor(main_node);

    return NULL;
}

ast_tree_elem_t *get_func_separated_init_args(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    ast_tree_elem_t *init_args = NULL;
    ast_tree_elem_t *copy_node = NULL;

    while (tl[*tp].token_type != TOKEN_C_BRACE) {
        copy_node = init_args;
        init_args = get_variable_initialization(data);
        CATCH_PARSE_ERROR(data, GET_FUNC_SEPARATED_INIT_ARGS, CLEAR_MEMORY(exit_mark))

        if (tl[*tp].token_type != TOKEN_COMMA && tl[*tp].token_type != TOKEN_C_BRACE){
            start_parser_err(data, tl[*tp], GET_FUNC_SEPARATED_INIT_ARGS);
            CLEAR_MEMORY(exit_mark);
        }

        if (tl[*tp].token_type == TOKEN_COMMA) {
            (*tp)++;
        }

        init_args = _COMMA(copy_node, init_args);
    }

    return init_args;

    exit_mark:

    sub_tree_dtor(init_args);
    sub_tree_dtor(copy_node);

    return NULL;
}

ast_tree_elem_t *get_grule_divided_list(parsing_block_t *data, int *empty, ast_tree_elem_t *(*grule_func)(parsing_block_t *data)) {
    assert(data);

    ast_tree_elem_t *copy_node = NULL;
    ast_tree_elem_t *right_node = NULL;

    ast_tree_elem_t *statement_node = NULL;

    *empty = 1;

    while ((statement_node = try_grule(data, grule_func))) {
        *empty = 0;
        copy_node = right_node;
        right_node = statement_node;

        STEP_OVER_TOKEN_WITH_CHECK(data, GET_GRULE_DIVIDED_LIST, TOKEN_SEMICOLON, CLEAR_MEMORY(exit_mark))

        right_node = _SEMICOLON(copy_node, right_node);
    }

    return right_node;

    exit_mark:

    sub_tree_dtor(copy_node);
    sub_tree_dtor(right_node);
    sub_tree_dtor(statement_node);

    return NULL;
}

ast_tree_elem_t *get_statement_list_untill_eof(parsing_block_t *data, int *empty) {
    assert(data);

    ast_tree_elem_t *copy_node = NULL;
    ast_tree_elem_t *right_node = NULL;

    *empty = 1;

    while (data->lexem_list[data->lexem_list_idx].token_type != TOKEN_EOF) {
        *empty = 0;
        copy_node = right_node;
        right_node = get_statement(data);
        CATCH_PARSE_ERROR(data, GET_SCOPE, CLEAR_MEMORY(exit_mark))

        STEP_OVER_TOKEN_WITH_CHECK(data, GET_SCOPE, TOKEN_SEMICOLON, CLEAR_MEMORY(exit_mark))

        right_node = _SEMICOLON(copy_node, right_node);
    }

    return right_node;

    exit_mark:

    sub_tree_dtor(copy_node);
    sub_tree_dtor(right_node);

    return NULL;
}

ast_tree_elem_t *get_scope(parsing_block_t *data, int *empty) {
    assert(data);
    ast_tree_elem_t *copy_node = NULL;
    ast_tree_elem_t *right_node = NULL;

    *empty = 1;

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SCOPE, TOKEN_O_FIG_BRACE, CLEAR_MEMORY(exit_mark));

    while (data->lexem_list[data->lexem_list_idx].token_type != TOKEN_C_FIG_BRACE) {
        *empty = 0;
        copy_node = right_node;
        right_node = get_statement(data);
        CATCH_PARSE_ERROR(data, GET_SCOPE, CLEAR_MEMORY(exit_mark))

        STEP_OVER_TOKEN_WITH_CHECK(data, GET_SCOPE, TOKEN_SEMICOLON, CLEAR_MEMORY(exit_mark))

        right_node = _SEMICOLON(copy_node, right_node);
    }

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_SCOPE, TOKEN_C_FIG_BRACE, CLEAR_MEMORY(exit_mark));

    return _SCOPE(right_node);

    exit_mark:

    sub_tree_dtor(copy_node);
    sub_tree_dtor(right_node);

    return NULL;
}

ast_tree_elem_t *get_function_initialization(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *return_type_node = NULL;
    ast_tree_elem_t *func_identificator = NULL;
    ast_tree_elem_t *args_node = NULL;
    ast_tree_elem_t *func_body_node = NULL;
    ast_tree_elem_t *statement_list = NULL;
    int empty = 0;

    return_type_node = get_type(data);
    CATCH_PARSE_ERROR(data, GET_FUNCTION_INITIALIZATION, CLEAR_MEMORY(exit_mark))

    func_identificator = get_func_identificator(data);
    CATCH_PARSE_ERROR(data, GET_FUNCTION_INITIALIZATION, CLEAR_MEMORY(exit_mark))


    STEP_OVER_TOKEN_WITH_CHECK(data, GET_FUNCTION_INITIALIZATION, TOKEN_O_BRACE, CLEAR_MEMORY(exit_mark))

    args_node = get_func_separated_init_args(data);
    CATCH_PARSE_ERROR(data, GET_FUNCTION_INITIALIZATION, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_FUNCTION_INITIALIZATION, TOKEN_C_BRACE, CLEAR_MEMORY(exit_mark))

    empty = 1;
    statement_list = get_scope(data, &empty);
    CATCH_PARSE_ERROR(data, GET_FUNCTION_INITIALIZATION, CLEAR_MEMORY(exit_mark))

    func_identificator->left = args_node;
    func_identificator->right = statement_list;

    return _FUNC_INIT(return_type_node, func_identificator);

    exit_mark:

    sub_tree_dtor(return_type_node);
    sub_tree_dtor(func_identificator);
    sub_tree_dtor(args_node);
    sub_tree_dtor(func_body_node);
    sub_tree_dtor(statement_list);

    return NULL;
}

ast_tree_elem_t *get_function_call(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *func_identificator = NULL;
    ast_tree_elem_t *args_node = NULL;
    ast_tree_elem_t *copy_node = NULL;

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    func_identificator = get_func_identificator(data);
    CATCH_PARSE_ERROR(data, GET_FUNCTION_CALL, CLEAR_MEMORY(exit_mark));

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_FUNCTION_CALL, TOKEN_O_BRACE, CLEAR_MEMORY(exit_mark));

    while (tl[*tp].token_type != TOKEN_C_BRACE) {
        copy_node = args_node;
        args_node = get_expression(data);
        CATCH_PARSE_ERROR(data, GET_FUNCTION_CALL, CLEAR_MEMORY(exit_mark))
        args_node = _COMMA(copy_node, args_node);

        if (tl[*tp].token_type == TOKEN_C_BRACE) {
            break;
        }

        STEP_OVER_TOKEN_WITH_CHECK(data, GET_FUNCTION_CALL, TOKEN_COMMA, CLEAR_MEMORY(exit_mark));
    }

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_FUNCTION_CALL, TOKEN_C_BRACE, CLEAR_MEMORY(exit_mark));

    return _CALL(func_identificator, args_node);

    exit_mark:

    sub_tree_dtor(func_identificator);
    sub_tree_dtor(args_node);
    sub_tree_dtor(copy_node);

    return NULL;
}

ast_tree_elem_t *get_assignment(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *left = NULL;
    ast_tree_elem_t *right = NULL;

    left = get_variable(data);
    CATCH_PARSE_ERROR(data, GET_ASSIGNMENT, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_ASSIGNMENT, TOKEN_ASSIGN, CLEAR_MEMORY(exit_mark))

    right = get_expression(data);
    CATCH_PARSE_ERROR(data, GET_ASSIGNMENT, CLEAR_MEMORY(exit_mark))

    return _ASSIGN(left, right);

    exit_mark:

    sub_tree_dtor(left);
    sub_tree_dtor(right);
    return NULL;
}

ast_tree_elem_t *get_primary_expression(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    if (tl[*tp].token_type == TOKEN_ID) {
        ast_tree_elem_t *id_node = get_variable(data);
        if (data->parser_err.err_state) {
            add_grule_to_parser_err(&data->parser_err, GET_PRIMARY_EXPRESSION);
            return id_node;
        }

        return id_node;
    } else if (tl[*tp].token_type == TOKEN_NUM_INT64 || tl[*tp].token_type == TOKEN_NUM_DOUBLE) {
        ast_tree_elem_t *const_node = get_constant(data);
        if (data->parser_err.err_state) {
            add_grule_to_parser_err(&data->parser_err, GET_PRIMARY_EXPRESSION);
            return const_node;
        }
        return const_node;
    } else if (tl[*tp].token_type == TOKEN_STR_LIT) {
        ast_tree_elem_t *str_lit_node = get_string_literal(data);
        if (data->parser_err.err_state) {
            add_grule_to_parser_err(&data->parser_err, GET_PRIMARY_EXPRESSION);
            return str_lit_node;
        }
        return str_lit_node;
    } else {
        start_parser_err(data, tl[*tp], GET_PRIMARY_EXPRESSION);
        return NULL;
    }
}

ast_tree_elem_t *get_while(parsing_block_t *data) {
    assert(data);

    ast_tree_elem_t *left_node = NULL;
    ast_tree_elem_t *right_node = NULL;
    int empty = 0;

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_WHILE, TOKEN_WHILE, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_WHILE, TOKEN_O_BRACE, CLEAR_MEMORY(exit_mark))

    left_node = get_expression(data);
    CATCH_PARSE_ERROR(data, GET_WHILE, CLEAR_MEMORY(exit_mark))

    STEP_OVER_TOKEN_WITH_CHECK(data, GET_WHILE, TOKEN_C_BRACE, CLEAR_MEMORY(exit_mark))

    empty = 1;
    right_node = get_scope(data, &empty);
    CATCH_PARSE_ERROR(data, GET_WHILE, CLEAR_MEMORY(exit_mark));
    if (empty) {
        start_parser_err(data, data->lexem_list[data->lexem_list_idx], GET_WHILE);
        CLEAR_MEMORY(exit_mark);
    }


    return _WHILE(left_node, right_node);

    exit_mark:

    sub_tree_dtor(left_node);
    sub_tree_dtor(right_node);

    return NULL;
}

ast_tree_elem_t *get_constant(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    lexem_t lexem = tl[*tp];
    lexer_token_t token_type = lexem.token_type;

    if (token_type != TOKEN_NUM_INT64 && token_type != TOKEN_NUM_DOUBLE) {
        start_parser_err(data, lexem, GET_CONSTANT);
        return NULL;
    }
    (*tp)++;

    if (token_type == TOKEN_NUM_INT64)  return _NUM_INT64(lexem.token_val.int64_val);
    else                                return _NUM_DOUBLE(lexem.token_val.double_val);
}

ast_tree_elem_t *get_string_literal(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);
    char * val = 0;
    size_t len = 0;

    if (tl[*tp].token_type != TOKEN_STR_LIT) {
        start_parser_err(data, tl[*tp], GET_CONSTANT);
        return NULL;
    }

    val = tl[*tp].token_val.string_val;
    len = tl[*tp].len;
    (*tp)++;

    return _STR_LIT(val, (int) len);
}

ast_tree_elem_t *get_variable(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    if (tl[*tp].token_type != TOKEN_ID) {
        start_parser_err(data, tl[*tp], GET_VARIABLE);
        return NULL;
    }
    int name_idx = tl[*tp].token_val.int64_val;
    char *var_name = data->name_table[tl[*tp].token_val.int64_val].name;
    (*tp)++;

    return _VAR(var_name, name_idx);
}

ast_tree_elem_t *get_func_identificator(parsing_block_t *data) {
    assert(data);

    lexem_t *tl = data->lexem_list;
    size_t *tp = &(data->lexem_list_idx);

    if (tl[*tp].token_type != TOKEN_ID) {
        start_parser_err(data, tl[*tp], GET_FUNC_IDENTIFICATOR);
        return NULL;
    }

    int func_name_id = tl[*tp].token_val.int64_val;
    char *func_name = data->name_table[tl[*tp].token_val.int64_val].name;

    (*tp)++;

    return _FUNC_ID(func_name, func_name_id);
}

#undef CATCH_PARSE_ERROR
#undef STEP_OVER_TOKEN_WITH_CHECK