#include <assert.h>
#include <cstring>
#include <stddef.h>
#include <stdlib.h>

#include "general.h"
#include "stack_err_proc.h"
#include "stack_output.h"

stk_err verify(stack_t *stk, stk_err *return_err, const char file_name[], const char func_name[], const int line_idx) {
    if (stk == NULL) {
        stk_add_err(return_err, STK_ERR_STACK_NULLPTR);
        goto dump_mark;
    }

    if (stk->data == NULL) {
        stk_add_err(return_err, STK_ERR_STACK_CONT_NULLPTR);
        goto dump_mark;
    }

    if (stk->size > stk->capacity) {
        stk_add_err(return_err, STK_ERR_STACK_OVERFLOW);
        goto dump_mark;
    }

    return STK_ERR_OK;

    dump_mark:

    DEBUG_STK_ERROR(*return_err);
    stack_dump(stk, stk->log_file_ptr, file_name, func_name, line_idx);

    return *return_err;
}

void stack_init(stack_t *stk, const size_t size, const size_t elem_nmemb, FILE* log_file_ptr, stk_err *return_err, const char born_file[], const int born_line, const char born_func[]) {
    if (stk == NULL) {
        stk_add_err(return_err,  STK_ERR_CALLOC);
        DEBUG_STK_ERROR(STK_ERR_CALLOC)
        CLEAR_MEMORY(exit_mark)
    }

    stk->size = 0;
    stk->capacity = size;
    stk->elem_nmemb = elem_nmemb;


    stk->data = (char *) calloc(stk->capacity, stk->elem_nmemb);

    if (stk->data == NULL) {
        stk_add_err(return_err, STK_ERR_CALLOC);
        DEBUG_STK_ERROR(STK_ERR_CALLOC)
        CLEAR_MEMORY(exit_mark)
    }

    stk->born_file = born_file;
    stk->born_line = born_line;
    stk->born_func = born_func;
    stk->log_file_ptr = log_file_ptr;

    return;

    exit_mark:

    if (stk->data != NULL) {
        FREE(stk->data);
    }

    return;
}

void stack_destroy(stack_t *stk) {
    if (stk == NULL) {
        return;
    }

    FREE(stk->data);
}

void resize(stack_t *stk, stk_err *return_err) {
    assert(stk != NULL);
    assert(return_err != NULL);

    bool resize_up_state = false;
    bool unit_length_state = false;

    if (stk->capacity == 0) {
        stk->capacity++;
        unit_length_state = true;
    } else if (stk->size >= stk->capacity) {
        stk->capacity *= resize_up_coeff;
        resize_up_state = true;
    } else if (stk->size <= stk->capacity / resize_down_check_coeff) {
        stk->capacity /= resize_down_coeff;
    } else {
        return;
    }

    stk->data = (char *) realloc(stk->data, stk->capacity * stk->elem_nmemb);

    if (stk->data == NULL) {
        stk_add_err(return_err, STK_ERR_REALLOC);
        DEBUG_STK_ERROR(STK_ERR_REALLOC);
        return;
    }

    if (resize_up_state) {
        size_t old_capacity = stk->capacity / resize_up_coeff;
        memset(stk->data + old_capacity * stk->elem_nmemb, 0, old_capacity * (resize_up_coeff - 1) * stk->elem_nmemb);
    }
    if (unit_length_state) {
        memset(stk->data, 0, 1 * stk->elem_nmemb);
    }

}

void stack_get_elem(stack_t *stk, void *dest, const size_t idx, stk_err *return_err) {
    assert(stk);
    assert(dest);

    if (idx >= stk->size) {
        stk_add_err(return_err, STK_ERR_INVALID_INDEX);
        debug("index {%lu} out of range. size = {%lu}", idx, stk->size);
        DEBUG_STK_ERROR(STK_ERR_INVALID_INDEX);
        return;
    }

    memcpy(dest, stk->data + idx * stk->elem_nmemb, stk->elem_nmemb);
}

bool stack_assign_elem(stack_t *stk, const size_t idx, void *value) {
    assert(stk);
    assert(value);

    if (idx >= stk->size) {
        debug("index {%lu} out of range. size = {%lu}", idx, stk->size);
        DEBUG_STK_ERROR(STK_ERR_INVALID_INDEX);
        return false;
    }

    memcpy(stk->data + idx * stk->elem_nmemb, value, stk->elem_nmemb);
    return true;
}

void stack_push(stack_t *stk, void *value, stk_err *return_err) {
    assert(stk);
    assert(value);

    VERIFY(stk, return_err, return)
    stk_err last_err = STK_ERR_OK;


    resize(stk, &last_err);
    if (last_err != STK_ERR_OK) {
        stk_add_err(return_err, last_err);
        DEBUG_STK_ERROR(last_err);
        return;
    }

    memcpy(stk->data + stk->size * stk->elem_nmemb, value, stk->elem_nmemb);
    stk->size++;

    VERIFY(stk, return_err, return)
}

void stack_pop(stack_t *stk, void *dest, stk_err *return_err) {
    assert(stk);

    stk_err last_err = STK_ERR_OK;

    VERIFY(stk, return_err, return)

    if (stk->size == 0) {
        stk_add_err(return_err, STK_ERR_STACK_POP);
        DEBUG_STK_ERROR(*return_err)
        fprintf(stk->log_file_ptr, "stk_err: pop underflow\n");
        return;
    }

    if (dest) {
        memcpy(dest, stk->data + (stk->size - 1) * stk->elem_nmemb, stk->elem_nmemb);
    }

    resize(stk, &last_err);
    if (last_err != STK_ERR_OK) {
        stk_add_err(return_err, last_err);
        DEBUG_STK_ERROR(last_err)
        return;
    }
    stk->size--;

    memset(stk->data + stk->size * stk->elem_nmemb, 0, stk->elem_nmemb);
}
