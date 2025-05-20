#ifndef STACK_FRAME_PROC_H
#define STACK_FRAME_PROC_H

#include "AST_structs.h"
#include "stack_funcs.h"
#include "translator_general.h"
#include "sections_processing.h"


enum cpu_stack_elem_types {
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

void cpu_elem_t_fprintf(FILE *stream, void *elem_ptr);
void dump_cpu_stack(FILE *stream, stack_t *cpu_stack);
int get_cpu_stack_elem_type_descr(char bufer[], const size_t buf_sz, const cpu_stack_elem_types type);

void cpu_stack_push_constant(stack_t *cpu_stack, const data_types data_type, const multi_val_t val);
bool cpu_stack_pop_value_for_variable(stack_t *cpu_stack, const data_types data_type);
bool cpu_stack_pop_return_addr(stack_t *cpu_stack);
void cpu_stack_push_return_addr(stack_t *cpu_stack);
void cpu_stack_push_variable(stack_t *cpu_stack, const data_types data_type, const char name[], const cpu_stack_elem_types type);
void cpu_stack_push_base_pointer(stack_t *cpu_stack);
bool cpu_stack_pop_base_pointer(stack_t *cpu_stack);
bool cpu_stack_pop_local_variable(stack_t *cpu_stack, const data_types data_type);

bool check_name_id_in_cur_scope(int name_id, stack_t *var_stack, const int cur_frame_ptr);

bool var_t_equal(const var_t v1, const var_t v2);
bool function_symbol_info_equal(const function_symbol_info v1, const function_symbol_info v2);
bool symbol_t_equal(const symbol_t v1, const symbol_t v2);

bool variable_symbol_info_equal(const variable_symbol_info v1, const variable_symbol_info v2);

var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_scope_deep);
bool var_stack_remove_local_variables(asm_glob_space *gl_space);

int add_local_var_into_frame(var_t var, asm_glob_space *gl_space);
void add_func_arg_into_frame(var_t var, asm_glob_space *gl_space, bool first_arg_in_rev_order_state);

bool check_prepared_for_operation_args(stack_t *cpu_stack);
bool check_elem_type_on_operation_supportive(cpu_stack_elem_types type);
bool check_cpu_stack_before_return(stack_t *cpu_stack);

#endif // STACK_FRAME_PROC_H