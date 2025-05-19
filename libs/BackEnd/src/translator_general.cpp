#include <stdio.h>
#include <assert.h>

#include "AST_structs.h"
#include "FrontEnd.h"
#include "general.h"
#include "lang_logger.h"
#include "string_funcs.h"
#include "stack_output.h"
#include "translator_general.h"

static char GLOBAL_BUFER[BUFSIZ] = {};


void var_t_fprintf(FILE *stream, void *elem_ptr) {
    var_t var = *(var_t *) elem_ptr;
    fprintf(stream, "LOC VAR INIT(loc_ram[%d], name='%s', id=%d, deep=%d)",
        var.stack_frame_idx, var.name, var.name_id, var.deep);
}

void dump_var_stack(FILE *stream, stack_t *var_stack) {
    assert(stream);
    assert(var_stack);

    fprintf_title(stream, "VAR_STACK", '=', STR_F_BORDER_SZ);
    fprintf(stream, "var_stack: \n");
    STACK_DUMP(var_stack, stream, var_t_fprintf);
}

data_types convert_lexer_token_data_type(lexer_token_t token_type) {
    switch (token_type) {
        case TOKEN_STRING_KEYWORD: return STRING_DATA_TYPE;
        case TOKEN_INT64_KEYWORD: return INT64_DATA_TYPE;
        case TOKEN_DOUBLE_KEYWORD: return DOUBLE_DATA_TYPE;
        default: token_write(GLOBAL_BUFER, BUFSIZ, token_type); debug("wrong data_type token : {%s}\n", GLOBAL_BUFER); abort();
    }
}

data_types_nmemb get_data_type_nmemb(data_types data_type) {
    switch (data_type) {
        case NONE_TYPE: return NONE_TYPE_NMEMB;
        case INT64_DATA_TYPE: return INT64_NMEMB;
        case DOUBLE_DATA_TYPE: return DOUBLE_NMEMB;
        case STRING_DATA_TYPE: return STRING_PTR_NMEMB;
    }
}

void dump_global_info(FILE *stream, asm_glob_space *gl_space) {
    assert(stream);

    fprintf_title(stream, "GLOBAL_INFO", '=', STR_F_BORDER_SZ);
    fprintf(stream,
                   "cur_scope_deep: %d\n"
                   "cur_frame: %d\n",
                   gl_space->cur_scope_deep, gl_space->cur_frame_ptr);

    fprintf(stream, "var_stack: \n");
    STACK_DUMP(&gl_space->var_stack, stream, var_t_fprintf);
}

void generate_mangled_name(char bufer[], const size_t buf_sz, const char prefix[], const size_t mangling_suf_sz) {
    assert(bufer);

    size_t prefix_sz = strnlen(prefix, buf_sz);
    snprintf(bufer, buf_sz, "%s", prefix);
    generate_random_string(bufer + prefix_sz, mangling_suf_sz);
}