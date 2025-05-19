#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sections_processing.h"
#include "string_funcs.h"
#include "translator_general.h"
#include "stack_frame_proc.h"

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

    return  v1.sym_type         == v2.sym_type          &&
            v1.sym_bind         == v2.sym_bind          &&
            v1.sym_section      == v2.sym_section       &&
            v1.section_offset   == v2.section_offset    &&
            v1.other_info       == v2.other_info;
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

    return v1.deep == v2.deep                               &&
           v1.stack_frame_idx == v2.stack_frame_idx         &&
           v1.name_id  == v2.deep                           &&
           name_eq;
}

var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    var_t var_info = {};

    if (var_stack->size == 0) {
        return POISON_VAR_T;
    }
    for (int i = (int) var_stack->size - 1; i >= cur_frame_ptr; i--) {
        stack_get_elem(var_stack, &var_info, (size_t) i);

        if (var_info.name_id == name_id) {
            return var_info;
        }
    }
    return POISON_VAR_T;
}

int add_var_into_frame(var_t var, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    var_t found_var = get_var_from_frame(var.name_id, var_stack, cur_frame_ptr);

    if (!var_t_equal(found_var, POISON_VAR_T)) {
        dump_var_stack(stderr, var_stack);
        RAISE_TRANSLATOR_ERROR("variable '%s' redefenition", var.name);
        return -1;
    }

    var_t prev_var = {};
    if (var_stack->size) {
        stack_get_elem(var_stack, &prev_var, var_stack->size - 1);
    } else {
        prev_var.stack_frame_idx = -1;
    }

    var.stack_frame_idx = prev_var.stack_frame_idx + 1;

    stack_push(var_stack, &var);
    return var.stack_frame_idx;
}

void var_stack_remove_local_variables(stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    var_t last_elem = {};

    for (int i = (int) var_stack->size; i > cur_frame_ptr; i--) {
        stack_get_elem(var_stack, &last_elem, var_stack->size - 1);
        stack_pop(var_stack);
    }
}

void var_stack_restore_old_frame(stack_t *var_stack, const int cur_frame_ptr) {
    var_stack_remove_local_variables(var_stack, cur_frame_ptr);
}

int get_stack_frame_var_offset(stack_t *var_stack, const size_t stack_frame_idx) {
    assert(var_stack);

    int variable_offset = 0;

    var_t last_elem = {};
    for (size_t i = 0; i < stack_frame_idx; i++) {
        stack_get_elem(var_stack, &last_elem, var_stack->size - 1);
        variable_offset += (int) last_elem.var_data_nmemb;
    }

    return variable_offset;
}
