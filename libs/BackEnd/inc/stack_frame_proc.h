#ifndef STACK_FRAME_PROC_H
#define STACK_FRAME_PROC_H

#include "stack_funcs.h"
#include "translator_general.h"

void dump_var_stack(FILE *stream, stack_t *var_stack);
var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr);
bool var_t_equal(const var_t v1, const var_t v2);
int add_var_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr);


#endif // STACK_FRAME_PROC_H