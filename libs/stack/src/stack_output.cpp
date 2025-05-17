#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <assert.h>

#include "general.h"
#include "stack_output.h"

void stack_fprintf_border(FILE* stream, const char bord_char, const size_t bord_sz, bool new_line) {
    for (size_t i = 0; i < bord_sz; i++) {
        fprintf(stream, WHT);
        fputc(bord_char, stream);
    }
    if (new_line) {
        fputc('\n', stream);
    }
}

void stack_fprintf_title(FILE *stream, const char tittle[], const char bord_char, const size_t bord_sz) {
    assert(tittle != NULL);
    size_t tittle_sz = strlen(tittle);
    if (bord_sz < tittle_sz) {
        return;
    }
    size_t len = bord_sz - tittle_sz;
    stack_fprintf_border(stream, bord_char, len / 2, false);
    fprintf_red(stream, "%s", tittle);
    stack_fprintf_border(stream, bord_char, (len + 1) / 2, true);
}

const char *get_log_descr(enum log_type_t log_type) {
    #define DESCR_(log_type) case log_type: return #log_type;

    switch (log_type) {
        DESCR_(LOG_ANALYS)
        DESCR_(LOG_DEBUG)
        DESCR_(LOG_ERROR)
        default: return "STRANGE LOG_TYPE";
    }
    #undef DESCR_
}

void print_log_time(FILE *log_output_file_ptr) {
    time_t cur_time;
    time(&cur_time);
    struct tm *now = localtime(&cur_time);

    char date_str[date_nmemb] = {};
    char time_str[time_nmemb] = {};
    strftime(date_str, date_nmemb, "%m/%d/%Y", now);
    strftime(time_str, time_nmemb, "%T", now);

    double milliseconds = ((double) clock()) / CLOCKS_PER_SEC;
    fprintf(log_output_file_ptr, "Date: %s Time: %s Ms: {%f}\n", date_str, time_str, milliseconds);
}

void print_log_border(FILE *log_output_file_ptr) {
    fprintf(log_output_file_ptr, "----------------------------------------------------------------------------------------------------\n");
}

void print_log_func_info(FILE *log_output_file_ptr, const char file_name[], const char func_name[], const int line_idx) {
    fprintf(log_output_file_ptr, "file : {%s}; func: {%s}; line: {%d}\n", file_name, func_name, line_idx);
}

void print_log_type(FILE *log_output_file_ptr, enum log_type_t log_type) {
    fprintf(log_output_file_ptr, "log_type: {%s}\n", get_log_descr(log_type));
}

void log_ptr_stack_dump(enum log_type_t log_type, FILE *log_output_file_ptr, stack_t *stk, const char file_name[], const char func_name[], const int line_idx) {
    print_log_border(log_output_file_ptr);
    print_log_type(log_output_file_ptr ,log_type);
    print_log_time(log_output_file_ptr);
    print_log_func_info(log_output_file_ptr, file_name, func_name, line_idx);
    print_log_border(log_output_file_ptr);

    fprintf(log_output_file_ptr, RED "------------------------------------------------------------\n" WHT);
    fprintf(log_output_file_ptr, GRN "_________stk: [%p:%p)" RED " | " WHT "bytes: %2lu\n" GRN, stk, stk + 1, sizeof(*stk));
    fprintf(log_output_file_ptr, RED "------------------------------------------------------------\n" WHT);

    fprintf(log_output_file_ptr, "________size: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->size, &stk->size + 1, sizeof(stk->size));
    fprintf(log_output_file_ptr, "____capacity: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->capacity, &stk->capacity + 1, sizeof(stk->capacity));

    fprintf(log_output_file_ptr, "_______*data: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->data, &stk->data + 1, sizeof(stk->data));
    fprintf(log_output_file_ptr, "__*born_file: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->born_file, &stk->born_file + 1, sizeof(stk->born_file));
    fprintf(log_output_file_ptr, "__*born_line: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->born_line, &stk->born_line + 1, sizeof(stk->born_line));
    fprintf(log_output_file_ptr, "__*born_func: [%p:%p)" RED " | " WHT "bytes: %2lu\n", &stk->born_func, &stk->born_func + 1, sizeof(stk->born_func));

    fprintf(log_output_file_ptr, RED "------------------------------------------------------------\n" WHT);

    print_log_border(log_output_file_ptr);
    fprintf(log_output_file_ptr, "\n");
}

void default_stk_elem_fprintf(FILE * stream, void *elem) {
    fprintf(stream, "{%d}", *(int *) elem);
}

void int_stack_output_func(FILE *stream, void *elem_ptr) {
    assert(stream);
    assert(elem_ptr);

    int elem_val = *(int *)(elem_ptr);
    printf("{%d}", elem_val);
}

void stack_dump(stack_t *stk, FILE* log_output_file_ptr, const char file_name[], const char func_name[], const int line_idx,
     void (*stk_elem_fprintf)(FILE *stream, void *elem_ptr)) {
    print_log_border(log_output_file_ptr);
    print_log_type(log_output_file_ptr, LOG_DEBUG);
    print_log_time(log_output_file_ptr);
    print_log_func_info(log_output_file_ptr, file_name, func_name, line_idx);
    print_log_border(log_output_file_ptr);

    if (stk == NULL) {
        return;
    }

    fprintf_red(log_output_file_ptr, "stack_t [%p] at %s:%d born at %s:%d(%s)\n", \
    stk, file_name, line_idx, stk->born_file, stk->born_line, stk->born_func);

    fprintf_wht(log_output_file_ptr, "{\n");

    fprintf_yel(log_output_file_ptr, "size = %lu\n", stk->size);
    // fprintf_yel(log_output_file_ptr, "size_ptr[%p]\n", &stk->size);
    fprintf_yel(log_output_file_ptr, "capacity = %lu\n", stk->capacity);
    // fprintf_yel(log_output_file_ptr, "capa_ptr[%p]\n", &stk->capacity);
    // fprintf_yel(log_output_file_ptr, "data_ptr[%p]\n", &stk->data);
    fprintf_yel(log_output_file_ptr, "stack[%p]\n", stk->data);

    fprintf_wht(log_output_file_ptr, "{\n");
    if (stk->data == NULL) {
        fprintf_red(log_output_file_ptr, "NULLPTR\n");
    } else {

        for (size_t i = 0; i < stk->size; i++) {
            fprintf_grn(log_output_file_ptr, "*[%lu] = ", i);
            stk_elem_fprintf(log_output_file_ptr, stk->data + i * stk->elem_nmemb);
            fprintf_grn(log_output_file_ptr, ";\n");
        }
        for (size_t i = stk->size; i < stk->capacity; i++) {
            fprintf_grn(log_output_file_ptr, "*[%lu] = EMPTY\n", i);
        }
    }
    fprintf_wht(log_output_file_ptr, "}\n");

    fprintf_wht(log_output_file_ptr, "}\n");

    print_log_border(log_output_file_ptr);
    fprintf(log_output_file_ptr, "\n");
}

void log_stk_err_print(enum log_type_t log_type, FILE *log_output_file_ptr, const stk_err err, const char file_name[], const char func_name[], const int line_idx) {
    print_log_border(log_output_file_ptr);
    print_log_type(log_output_file_ptr, log_type);
    print_log_time(log_output_file_ptr);
    print_log_func_info(log_output_file_ptr, file_name, func_name, line_idx);
    print_log_border(log_output_file_ptr);

    fprintf(log_output_file_ptr, "-----------ERROR_LIST------------------\n");
    for (size_t stk_err_bit = 0; stk_err_bit < 32; stk_err_bit++) {
        if ((err >> stk_err_bit) & 1ull) {
            fprintf(log_output_file_ptr, "%s\n", stkerr_get_bit_descr((stk_err) (1ull << stk_err_bit)));
        }
    }
    print_log_border(log_output_file_ptr);
    fprintf(log_output_file_ptr, "\n");
}

void log_var_print(enum log_type_t log_type, FILE *log_output_file_ptr, const char file_name[], const char func_name[], const int line_idx, const char fmt[], ...) {
    print_log_border(log_output_file_ptr);
    print_log_type(log_output_file_ptr, log_type);
    print_log_time(log_output_file_ptr);
    print_log_func_info(log_output_file_ptr, file_name, func_name, line_idx);
    print_log_border(log_output_file_ptr);

    if (line_idx) {}

    va_list args;
    va_start(args, fmt);
    vfprintf(log_output_file_ptr, fmt, args);

    va_end(args);
    print_log_border(log_output_file_ptr);
    fprintf(log_output_file_ptr, "\n");
}