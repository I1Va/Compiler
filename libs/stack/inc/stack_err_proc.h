#ifndef STK_ERR_PROC_H
#define STK_ERR_PROC_H

enum stk_err {
    STK_ERR_OK                         = 0,
    STK_ERR_NULLPTR                    = 1 << 0,
    STK_ERR_CALLOC                     = 1 << 1,
    STK_ERR_MEM                        = 1 << 2,
    STK_ERR_UNKNOWN                    = 1 << 3,
    STK_ERR_STAT                       = 1 << 4,
    STK_ERR_INPUT_DATA                 = 1 << 5,
    STK_ERR_FILE_OPEN                  = 1 << 6,
    STK_ERR_FILE_CLOSE                 = 1 << 7,
    STK_ERR_ARGS                       = 1 << 8,
    STK_ERR_WRONG_COEF                 = 1 << 9,
    STK_ERR_INIT                       = 1 << 10,
    STK_ERR_STACK_NULLPTR              = 1 << 11,
    STK_ERR_STACK_CONT_NULLPTR         = 1 << 12,
    STK_ERR_STACK_OVERFLOW             = 1 << 13,
    STK_ERR_STACK_POP                  = 1 << 14,
    STK_ERR_REALLOC                    = 1 << 15,
    STK_ERR_CANARY_LEFT                = 1 << 16,
    STK_ERR_CANARY_MID                 = 1 << 17,
    STK_ERR_CANARY_RIGHT               = 1 << 18,
    STK_ERR_CANARY_STK_RIGHT           = 1 << 19,
    STK_ERR_HASH_STACK_DATA_MISMATCH   = 1 << 20,
    STK_ERR_CANARY_STK_LEFT            = 1 << 21,
    STK_ERR_SYSTEM                     = 1 << 22,
    STK_ERR_STACK_LAST_ELEM            = 1 << 23,
    STK_ERR_HASH_STACK_STRUCT_MISMATCH = 1 << 24,
    STK_ERR_INVALID_INDEX              = 1 << 25,
};

const char *stkerr_get_descr(stk_err err);
const char *stkerr_get_bit_descr(stk_err err);
int fprintf_abort(const char file_name[], const int line, const char function_name[], const char error_descr[]);
void stk_add_err(stk_err *src, stk_err add);

#ifndef NDEBUG
    #define DEBUG_STK_ERROR(code) fprintf_red(stderr, "{%s} [%s: %d]: descr{%s}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, stkerr_get_descr(code));
    #define MY_ASSERT(stk_err, exit_instruction) {DEBUG_STK_ERROR(stk_err); exit_instruction;};

#else
    #define DEBUG_STK_ERROR(code) ;
    #define ON_DEBUG(...)
    #define MY_ASSERT(stk_err, exit_instruction) ;
#endif // NDEBUG

#endif // STK_ERR_PROC_H
