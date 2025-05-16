#include "stack_err_proc.h"

const char *stkerr_get_bit_descr(stk_err err) {
    #define BIT_DESCR_(code) case code : return #code;
    switch (err) {
        BIT_DESCR_(STK_ERR_OK)
        BIT_DESCR_(STK_ERR_UNKNOWN)
        BIT_DESCR_(STK_ERR_CALLOC)
        BIT_DESCR_(STK_ERR_NULLPTR)
        BIT_DESCR_(STK_ERR_STAT)
        BIT_DESCR_(STK_ERR_INPUT_DATA)
        BIT_DESCR_(STK_ERR_MEM)
        BIT_DESCR_(STK_ERR_FILE_CLOSE)
        BIT_DESCR_(STK_ERR_FILE_OPEN)
        BIT_DESCR_(STK_ERR_ARGS)
        BIT_DESCR_(STK_ERR_WRONG_COEF)
        BIT_DESCR_(STK_ERR_INIT)
        BIT_DESCR_(STK_ERR_STACK_NULLPTR)
        BIT_DESCR_(STK_ERR_STACK_CONT_NULLPTR)
        BIT_DESCR_(STK_ERR_STACK_OVERFLOW)
        BIT_DESCR_(STK_ERR_STACK_POP)
        BIT_DESCR_(STK_ERR_REALLOC)
        BIT_DESCR_(STK_ERR_CANARY_LEFT)
        BIT_DESCR_(STK_ERR_CANARY_MID)
        BIT_DESCR_(STK_ERR_CANARY_RIGHT)
        BIT_DESCR_(STK_ERR_CANARY_STK_RIGHT)
        BIT_DESCR_(STK_ERR_HASH_STACK_DATA_MISMATCH)
        BIT_DESCR_(STK_ERR_CANARY_STK_LEFT)
        BIT_DESCR_(STK_ERR_SYSTEM)
        BIT_DESCR_(STK_ERR_STACK_LAST_ELEM)
        BIT_DESCR_(STK_ERR_HASH_STACK_STRUCT_MISMATCH)
        BIT_DESCR_(STK_ERR_INVALID_INDEX)

        default: return "VERY STRANGE ERROR:(";
    }
    #undef BIT_DESCR_
}

const char *stkerr_get_descr(stk_err err) {
    #define DESCR_(code, err)           \
    {                                   \
        if (code & err) {               \
            return #err;                \
        }                               \
    }                                   \

    DESCR_(err, STK_ERR_NULLPTR)
    DESCR_(err, STK_ERR_CALLOC)
    DESCR_(err, STK_ERR_UNKNOWN)
    DESCR_(err, STK_ERR_STAT)
    DESCR_(err, STK_ERR_INPUT_DATA)
    DESCR_(err, STK_ERR_FILE_OPEN)
    DESCR_(err, STK_ERR_FILE_CLOSE)
    DESCR_(err, STK_ERR_ARGS)
    DESCR_(err, STK_ERR_WRONG_COEF)
    DESCR_(err, STK_ERR_INIT)
    DESCR_(err, STK_ERR_STACK_NULLPTR)
    DESCR_(err, STK_ERR_STACK_CONT_NULLPTR)
    DESCR_(err, STK_ERR_STACK_OVERFLOW)
    DESCR_(err, STK_ERR_STACK_POP)
    DESCR_(err, STK_ERR_REALLOC)

    DESCR_(err, STK_ERR_CANARY_MID)
    DESCR_(err, STK_ERR_CANARY_RIGHT)
    DESCR_(err, STK_ERR_CANARY_STK_RIGHT)
    DESCR_(err, STK_ERR_CANARY_STK_LEFT)

    DESCR_(err, STK_ERR_HASH_STACK_DATA_MISMATCH)
    DESCR_(err, STK_ERR_HASH_STACK_STRUCT_MISMATCH)
    DESCR_(err, STK_ERR_STACK_LAST_ELEM)

    DESCR_(err, STK_ERR_SYSTEM)

    if (err == STK_ERR_OK) {
        return "STK_ERR_OK";
    }

    return "STRANGE ERROR!";
}

void stk_add_err(stk_err *src, stk_err add) {
    if (src) {
        *src = (stk_err)((unsigned long long)(*src) | (unsigned long long) add);
    }
}

#undef DESCR_