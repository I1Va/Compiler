#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lang_logger.h"
#include "sections_processing.h"
#include "stack_funcs.h"
#include "string_funcs.h"
#include "translator_general.h"
#include "stack_frame_proc.h"
#include "stack_funcs.h"
#include "stack_output.h"
#include "sections_processing.h"



static char GLOBAL_BUFER[BUFSIZ] = {};


void cpu_elem_t_fprintf(FILE *stream, void *elem_ptr) {
    cpu_stack_elem_t elem = *(cpu_stack_elem_t *) elem_ptr;

    int global_bufer_offset = 0;

    char *type_str_ptr = GLOBAL_BUFER + global_bufer_offset;
    global_bufer_offset += 1 + get_cpu_stack_elem_type_descr(GLOBAL_BUFER + global_bufer_offset, BUFSIZ, elem.stack_elem_type);

    char *data_type_str_ptr = GLOBAL_BUFER + global_bufer_offset;
    global_bufer_offset += 1 + get_data_type_descr(GLOBAL_BUFER + global_bufer_offset, BUFSIZ, elem.data_type);

    char *data_value_str_ptr = GLOBAL_BUFER + global_bufer_offset;
    global_bufer_offset += 1 + get_multi_val_t_descr(GLOBAL_BUFER + global_bufer_offset, BUFSIZ, elem.data_value);

    fprintf(stream, "name:`%s`, type:`%s`, data_type:`%s`, data_value:`%s`",
        elem.name, type_str_ptr, data_type_str_ptr, data_value_str_ptr);
}

void dump_cpu_stack(FILE *stream, stack_t *cpu_stack) {
    assert(stream);
    assert(cpu_stack);

    fprintf_title(stream, "CPU_STACK", '=', STR_F_BORDER_SZ);
    STACK_DUMP(cpu_stack, stream, cpu_elem_t_fprintf);
    fprintf_title(stream, "CPU_STACK", '=', STR_F_BORDER_SZ);
    printf("\n");
}

int get_cpu_stack_elem_type_descr(char bufer[], const size_t buf_sz, const cpu_stack_elem_types type) {
    assert(bufer);
    #define T_DESCR_(type) case type: return snprintf(bufer, buf_sz, "%s", #type);
    switch (type) {
        T_DESCR_(CPU_STACK_UNINITIALIZED_VAR)
        T_DESCR_(CPU_STACK_VAR_VALUE)
        T_DESCR_(CPU_STACK_CONSTANT)
        T_DESCR_(CPU_STACK_RETURN_ADDR)
        T_DESCR_(CPU_STACK_BASE_POINTER)
        default: return snprintf(bufer, buf_sz, "UNKNOWN_CPU_STACK_ELEM_TYPE (%d)", type);
    }
    return 0;
}

void cpu_stack_push_constant(stack_t *cpu_stack, const data_types data_type, const multi_val_t val) {
    assert(cpu_stack);

    cpu_stack_elem_t cur_cpu_stack_elem = {};
    cur_cpu_stack_elem.name = NULL;
    cur_cpu_stack_elem.stack_elem_type = CPU_STACK_CONSTANT;
    cur_cpu_stack_elem.data_type = data_type;
    cur_cpu_stack_elem.data_value = val;

    stack_push(cpu_stack, &cur_cpu_stack_elem);
}

bool cpu_stack_pop_value_for_variable(stack_t *cpu_stack, const data_types data_type) {
    assert(cpu_stack);

    if (cpu_stack->size == 0) {
        debug("cpu_stack->size == 0\n");
        return false;
    }

    cpu_stack_elem_t last_elem = {};
    stack_get_elem(cpu_stack, &last_elem, cpu_stack->size - 1);
    stack_pop(cpu_stack);

    if (!(last_elem.stack_elem_type == CPU_STACK_VAR_VALUE || last_elem.stack_elem_type == CPU_STACK_CONSTANT)) {
        get_cpu_stack_elem_type_descr(GLOBAL_BUFER, BUFSIZ, last_elem.stack_elem_type);
        debug("(cpu_stack_elem_types) -> expected : CPU_STACK_CONSTANT, got : {%s}", GLOBAL_BUFER);
        dump_cpu_stack(stderr, cpu_stack);
        return false;
    }


    if (last_elem.data_type != data_type) {
        int bufer_offset = get_data_type_descr(GLOBAL_BUFER, BUFSIZ, data_type);
        get_data_type_descr(GLOBAL_BUFER + bufer_offset + 1, BUFSIZ, last_elem.data_type);
        debug("(data_types) -> expected : {%s}, got : {%s}", GLOBAL_BUFER, GLOBAL_BUFER + bufer_offset + 1);

        dump_cpu_stack(stderr, cpu_stack);
        return false;
    }

    return true;
}

bool cpu_stack_pop_return_addr(stack_t *cpu_stack) {
    assert(cpu_stack);

    if (cpu_stack->size == 0) {
        debug("cpu_stack->size == 0\n");
        return false;
    }

    cpu_stack_elem_t last_elem = {};
    stack_get_elem(cpu_stack, &last_elem, cpu_stack->size - 1);
    stack_pop(cpu_stack);

    if (last_elem.stack_elem_type != CPU_STACK_RETURN_ADDR) {
        get_cpu_stack_elem_type_descr(GLOBAL_BUFER, BUFSIZ, last_elem.stack_elem_type);
        debug("(cpu_stack_elem_types) -> expected : CPU_STACK_RETURN_ADDR, got : {%s}", GLOBAL_BUFER);
        dump_cpu_stack(stderr, cpu_stack);
        return false;
    }

    return true;
}

void cpu_stack_push_return_addr(stack_t *cpu_stack) {
    assert(cpu_stack);

    cpu_stack_elem_t cur_cpu_stack_elem = {};
    cur_cpu_stack_elem.name = NULL;
    cur_cpu_stack_elem.stack_elem_type = CPU_STACK_RETURN_ADDR;
    cur_cpu_stack_elem.data_type = INT64_DATA_TYPE;
    cur_cpu_stack_elem.data_value = {};

    stack_push(cpu_stack, &cur_cpu_stack_elem);
}

void cpu_stack_push_variable(stack_t *cpu_stack, const data_types data_type, const char name[], const cpu_stack_elem_types type) {
    assert(cpu_stack);


    cpu_stack_elem_t cur_cpu_stack_elem = {};
    cur_cpu_stack_elem.name = name;
    cur_cpu_stack_elem.stack_elem_type = type;
    cur_cpu_stack_elem.data_type = data_type;
    cur_cpu_stack_elem.data_value = {};

    stack_push(cpu_stack, &cur_cpu_stack_elem);
}

void cpu_stack_push_base_pointer(stack_t *cpu_stack) {
    assert(cpu_stack);

    cpu_stack_elem_t cur_cpu_stack_elem = {};
    cur_cpu_stack_elem.name = NULL;
    cur_cpu_stack_elem.stack_elem_type = CPU_STACK_BASE_POINTER;
    cur_cpu_stack_elem.data_type = INT64_DATA_TYPE;
    cur_cpu_stack_elem.data_value = {};

    stack_push(cpu_stack, &cur_cpu_stack_elem);
}

bool cpu_stack_pop_base_pointer(stack_t *cpu_stack) {
    assert(cpu_stack);

    if (cpu_stack->size == 0) {
        debug("cpu_stack->size == 0\n");
        return false;
    }


    cpu_stack_elem_t last_elem = {};
    stack_get_elem(cpu_stack, &last_elem, cpu_stack->size - 1);
    stack_pop(cpu_stack);




    if (last_elem.stack_elem_type != CPU_STACK_BASE_POINTER) {
        get_cpu_stack_elem_type_descr(GLOBAL_BUFER, BUFSIZ, last_elem.stack_elem_type);
        debug("(cpu_stack_elem_types) -> expected : CPU_STACK_CONSTANT, got : {%s}", GLOBAL_BUFER);

        dump_cpu_stack(stderr, cpu_stack);
        return false;
    }

    return true;
}






static bool check_locals_untill_var(int name_id, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    if (var_stack->size == 0) return false;

    var_t cur_var = {};

    bool meet_local_var_in_frame = false;

    for (int i = cur_frame_ptr; i < var_stack->size; i++) {
        stack_get_elem(var_stack, &cur_var, (size_t) i);

        meet_local_var_in_frame |= (cur_var.var_type == VAR_TYPE_LOCAL_VAR);
        if (cur_var.name_id == name_id) return meet_local_var_in_frame;
    }

    return meet_local_var_in_frame;
}

bool check_name_id_in_cur_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    if (var_stack->size == 0) return false;

    var_t cur_var = {};
    for (int i = cur_frame_ptr; i < var_stack->size; i++) {
        stack_get_elem(var_stack, &cur_var, (size_t) i);

        if (cur_var.name_id == name_id) return true;
    }

    return false;
}

bool var_t_equal(const var_t v1, const var_t v2) {
    if (v1.name == NULL && v2.name != NULL) {
        return false;
    }
    if (v1.name != NULL && v2.name == NULL) {
        return false;
    }

    bool name_eq = (v1.name == NULL) && (v2.name == NULL);
    if (v1.name != NULL && v2.name != NULL) {
        name_eq = name_eq || strcmp(v1.name, v2.name);
    }

    return v1.var_data_type == v2.var_data_type                 &&
            v1.var_data_nmemb == v2.var_data_nmemb              &&
            v1.name_id == v2.name_id                            &&
            name_eq                                             &&
            v1.var_type == v2.var_type                          &&
            v1.base_pointer_offset == v2.base_pointer_offset    &&
            v1.deep == v2.deep;
}

var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_scope_deep) {
    assert(var_stack);

    if (var_stack->size == 0) return POISON_VAR_T;

    var_t cur_var = {};
    cur_var.deep = -1;

    for (int i = var_stack->size - 1; i >= 0 && cur_var.deep <= cur_scope_deep; i--) {
        stack_get_elem(var_stack, &cur_var, (size_t) i);
        if (cur_var.name_id == name_id) return cur_var;

    }

    return POISON_VAR_T;
}

bool function_symbol_info_equal(const function_symbol_info v1, const function_symbol_info v2) {
    return v1.return_data_type == v2.return_data_type       &&
            v1.args_cnt == v2.args_cnt                      &&
            v1.args_summary_nmemb == v2.args_summary_nmemb;
}

bool variable_symbol_info_equal(const variable_symbol_info v1, const variable_symbol_info v2) {
    return v1.var_data_type == v2.var_data_type;
}

bool symbol_t_equal(const symbol_t v1, const symbol_t v2) {
    if (v1.sym_name == NULL && v2.sym_name != NULL) {
        return false;
    }
    if (v1.sym_name != NULL && v2.sym_name == NULL) {
        return false;
    }

    bool name_eq = (v1.sym_name == NULL) && (v1.sym_name);

    if (v1.sym_name != NULL && v2.sym_name != NULL) {
        name_eq = name_eq || strcmp(v1.sym_name, v2.sym_name);
    }

    return  v1.sym_type         == v2.sym_type                             &&
            v1.sym_bind         == v2.sym_bind                             &&
            v1.sym_section      == v2.sym_section                          &&
            v1.section_offset   == v2.section_offset                       &&
            function_symbol_info_equal(v1.func_sym_info, v2.func_sym_info) &&
            variable_symbol_info_equal(v1.var_sym_info, v2.var_sym_info);
}


void add_func_arg_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr, bool first_arg_in_rev_order) {
    assert(var_stack);

    if (check_name_id_in_cur_frame(var.name_id, var_stack, cur_frame_ptr)) {
        dump_var_stack(stderr, var_stack);
        RAISE_TRANSLATOR_ERROR("variable '%s' redefenition", var.name);
        return;
    }
    if (check_locals_untill_var(var.name_id, var_stack, cur_frame_ptr) && var.var_type == VAR_TYPE_FUNCTION_ARG) {
        RAISE_TRANSLATOR_ERROR("met local vars in stack frame before func arg `%s`", var.name)
    }

    if (!first_arg_in_rev_order) {
        assert(var_stack->size > 0);

        var_t last_arg_var = {};
        stack_get_elem(var_stack, &last_arg_var, var_stack->size - 1);

        var.base_pointer_offset = last_arg_var.base_pointer_offset - var.var_data_nmemb;
    }

    stack_push(var_stack, &var);
}

void add_local_var_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    if (check_name_id_in_cur_frame(var.name_id, var_stack, cur_frame_ptr)) {
        dump_var_stack(stderr, var_stack);
        RAISE_TRANSLATOR_ERROR("variable '%s' redefenition", var.name);
        return;
    }

    if (var_stack->size) {
        var_t prev_var = {};
        stack_get_elem(var_stack, &prev_var, var_stack->size - 1);
        var.base_pointer_offset = prev_var.base_pointer_offset - var.var_data_nmemb;
    } else {
        var.base_pointer_offset = 0 - (int) var.var_data_nmemb;
    }

    stack_push(var_stack, &var);
}

void var_stack_remove_local_variables(stack_t *var_stack, const int cur_scope_deep) {
    assert(var_stack);

    if (!var_stack->size) return;

    var_t last_elem = {};
    stack_get_elem(var_stack, &last_elem, var_stack->size - 1);
    while (last_elem.deep > cur_scope_deep && var_stack->size) {
        stack_get_elem(var_stack, &last_elem, var_stack->size - 1);
        stack_pop(var_stack);
    }
}

// int get_stack_frame_var_offset(stack_t *var_stack, const size_t stack_frame_idx) {
//     assert(var_stack);

//     int variable_offset = 0;

//     var_t last_elem = {};
//     for (size_t i = 0; i < stack_frame_idx; i++) {
//         stack_get_elem(var_stack, &last_elem, var_stack->size - 1);
//         variable_offset += (int) last_elem.var_data_nmemb;
//     }

//     return variable_offset;
// }
