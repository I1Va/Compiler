#include <stdio.h>
#include <assert.h>

#include "general.h"
#include "string_funcs.h"
#include "stack_output.h"
#include "translator_general.h"

void var_t_fprintf(FILE *stream, void *elem_ptr) {

    var_t var = *(var_t *) elem_ptr;
    fprintf(stream, "LOC VAR INIT(loc_ram[%d], name='%s', id=%d, deep=%d)",
        var.loc_addr, var.name, var.name_id, var.deep);
}

void dump_var_stack(FILE *stream, stack_t *var_stack) {
    assert(stream);
    assert(var_stack);

    fprintf_title(stream, "VAR_STACK", '=', STR_F_BORDER_SZ);
    fprintf(stream, "var_stack: \n");
    STACK_DUMP(var_stack, stream, var_t_fprintf);
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
