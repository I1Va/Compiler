#include <assert.h>

#include "stack_frame_proc.h"


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

    return v1.deep == v2.deep               &&
           v1.loc_addr == v2.loc_addr       &&
           v1.name_id  == v2.deep           &&
           name_eq                          &&
           v1.type == v2.type;
}

var_t get_var_from_frame(int name_id, stack_t *var_stack, const int cur_frame_ptr) {
    assert(var_stack);

    var_t var_info = {};

    if (var_stack->size == 0) {
        return POISON_VAR;
    }
    for (int i = (int) var_stack->size - 1; i >= cur_frame_ptr; i--) {
        stack_get_elem(var_stack, &var_info, (size_t) i);

        if (var_info.name_id == name_id) {
            return var_info;
        }
    }
    return POISON_VAR;
}