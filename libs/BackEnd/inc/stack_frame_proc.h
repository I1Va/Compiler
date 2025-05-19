#ifndef STACK_FRAME_PROC_H
#define STACK_FRAME_PROC_H

#include "stack_funcs.h"
#include "translator_general.h"
#include "sections_processing.h"


void dump_var_stack(FILE *stream, stack_t *var_stack);
var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr);
bool var_t_equal(const var_t v1, const var_t v2);
int add_var_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr);
void var_stack_restore_old_frame(stack_t *var_stack, const int cur_frame_ptr);
int get_stack_frame_var_offset(stack_t *var_stack, const size_t stack_frame_idx);
bool symbol_t_equal(const symbol_t v1, const symbol_t v2);
void var_stack_remove_local_variables(stack_t *var_stack, const int cur_frame_ptr);


#endif // STACK_FRAME_PROC_H