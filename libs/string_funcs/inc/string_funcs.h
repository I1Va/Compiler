#ifndef STRING_FUNCS_H
#define STRING_FUNCS_H

#include "general.h"
#include <string.h>
#include <stdio.h>

struct str_t {
    char *str_ptr;
    size_t len;
};

struct str_storage_t {
    void *prev_chunk; // str_storage_t *

    size_t idx;
    size_t chunk_size;

    char *data;
};

str_storage_t *str_storage_t_ctor(const size_t chunk_size);
void str_storage_t_dtor(str_storage_t *storage);
char *get_new_str_ptr(str_storage_t **storage, const size_t len);

str_t read_text_from_file(const char *path);
bool write_text_to_file(const str_t *text, const char *path);

int get_file_sz(const char *path);
void remove_chars_from_text(str_t *text, const char sims[]);

size_t get_max_str_len(size_t n, ...);
size_t get_max_str_len_from_arr(size_t n, char **name_table);

void fprintf_str_block(FILE *stream, const size_t indent_sz, const size_t block_sz, const char *string);
void fprintf_indent_str(FILE *stream, const size_t indent_sz, const char fmt[], ...);
void fprintf_border(FILE* stream, const char bord_char, const size_t bord_sz, bool new_line);
void fprintf_title(FILE *stream, const char tittle[], const char bord_char, const size_t bord_sz);

void fprintf_bin(FILE *stream, int mask);
void get_bit_str(int mask, char res_str[], const size_t n_bits);
#define debug(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: descr{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);

#define MESSAGE_PRINT(str_, ...)                            \
{                                                           \
    fprintf_title(stdout, "MESSAGE", '=', STR_F_BORDER_SZ); \
    fprintf(str_, ...)                                      \
    fprintf_border(stdout, '=', STR_F_BORDER_SZ, true);     \
}                                                           \


#endif // STRING_FUNCS_H
