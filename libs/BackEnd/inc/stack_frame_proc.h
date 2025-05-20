#ifndef STACK_FRAME_PROC_H
#define STACK_FRAME_PROC_H

#include "AST_structs.h"
#include "stack_funcs.h"
#include "translator_general.h"
#include "sections_processing.h"


enum cpu_stack_elem_types {
    CPU_STACK_UNINITIALIZED_VAR,
    CPU_STACK_VAR_VALUE,
    CPU_STACK_CONSTANT,
    CPU_STACK_RETURN_ADDR,
    CPU_STACK_BASE_POINTER,
};

struct cpu_stack_elem_t {
    const char *name;
    cpu_stack_elem_types stack_elem_type;
    data_types data_type;
    multi_val_t data_value;
};


int get_cpu_stack_elem_type_descr(char bufer[], const size_t buf_sz, const cpu_stack_elem_types type);

void dump_var_stack(FILE *stream, stack_t *var_stack);
var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr);
bool var_t_equal(const var_t v1, const var_t v2);
int add_var_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr);
void var_stack_restore_old_frame(stack_t *var_stack, const int cur_frame_ptr);
int get_stack_frame_var_offset(stack_t *var_stack, const size_t stack_frame_idx);
bool symbol_t_equal(const symbol_t v1, const symbol_t v2);
void var_stack_remove_local_variables(stack_t *var_stack, const int cur_frame_ptr);

void cpu_stack_push_constant(stack_t *cpu_stack, const data_types data_type, const multi_val_t val);
bool cpu_stack_pop_constant(stack_t *cpu_stack, const data_types data_type);

bool cpu_stack_pop_base_pointer(stack_t *cpu_stack);
void cpu_stack_push_base_pointer(stack_t *cpu_stack);

void cpu_stack_push_variable(stack_t *cpu_stack, const data_types data_type, const char name[], const cpu_stack_elem_types type);

int get_cpu_stack_elem_type_descr(char bufer[], const size_t buf_sz, const cpu_stack_elem_types type);
void dump_cpu_stack(FILE *stream, stack_t *cpu_stack);

#endif // STACK_FRAME_PROC_H