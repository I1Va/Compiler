#ifndef STACK_FUNCS_H
#define STACK_FUNCS_H

#include <stdio.h>
#include <string.h>

#include "stack_err_proc.h"

struct stack_t {
    size_t size;
    size_t capacity;

    size_t elem_nmemb;

    char *data;

    FILE *log_file_ptr;
    const char *born_file;
    int born_line;
    const char *born_func;
};

const size_t resize_up_coeff = 2;
const size_t resize_down_check_coeff = 4;
const size_t resize_down_coeff = 2;

const size_t dump_output_sz = 10;

void stack_destroy(stack_t *stk);
stk_err verify(stack_t *stk, stk_err *return_err, const char file_name[], const char func_name[], const int line_idx);
void stack_init(stack_t *stk, const size_t size, const size_t elem_nmemb, FILE* log_file_ptr, stk_err *return_err=NULL, const char born_file[]="", const int born_line=0, const char born_func[]="");
void resize(stack_t *stk, stk_err *return_err=NULL);
void stack_push(stack_t *stk, void *value, stk_err *return_err=NULL);
bool stack_assign_elem(stack_t *stk, const size_t idx, void *value);
void stack_get_elem(stack_t *stk, void *dest, const size_t idx, stk_err *return_err=NULL);
void stack_pop(stack_t *stk, void *dest=NULL, stk_err *return_err=NULL);

#ifndef NDEBUG
    #define VERIFY(stk, return_err, exit_instruction)                                         \
        {                                                                                             \
            if (verify(stk, return_err, __FILE__, __PRETTY_FUNCTION__, __LINE__) != STK_ERR_OK) { \
                exit_instruction;                                                                     \
            }                                                                                          \
        }
    #define STACK_INIT(stk, size, nmemb, log_file_ptr, return_err) stack_init(stk, size, nmemb, log_file_ptr, return_err, __FILE__, __LINE__, __PRETTY_FUNCTION__);

#else
    #define VERIFY(stk, return_err, exit_instruction) ;
    #define STACK_INIT(stk, size, nmemb, log_file_ptr, return_err) stack_init(stk, size, nmemb, log_file_ptr, return_err);
#endif // NDEBUG

#endif // STACK_FUNCS_H