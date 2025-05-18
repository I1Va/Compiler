#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "general.h"
#include "string_funcs.h"

str_storage_t *str_storage_t_ctor(const size_t chunk_size) {
    str_storage_t *storage = (str_storage_t *) calloc(1, sizeof(str_storage_t));

    storage->idx = 0;
    storage->chunk_size = chunk_size;
    storage->prev_chunk = NULL;

    storage->data = (char *) calloc(chunk_size, sizeof(char));
    if (storage->data == NULL) {
        debug("storage->data calloc failed")
        return NULL;
    }

    return storage;
}

void generate_random_string(char bufer[], const size_t str_sz) {
    for (size_t i = 0; i < str_sz; i++) {
        bufer[i] = 'a' + rand() % ENGLISH_ALPHABET_SZ;
    }
    bufer[str_sz] = '\0';
}

size_t get_max_str_len(size_t n, ...) {
    size_t max_str_len = 0;
    va_list factor;
    va_start(factor, n);

    for(size_t i = 0; i < n; i++) {
        char *str_ptr = va_arg(factor, char *); // FIXME: почему, при strlen(va_arg(factor, char *)) возникает SEGF?
        max_str_len = MAX(max_str_len, strlen(str_ptr));
    }

    va_end(factor);
    return max_str_len;
}

size_t get_max_str_len_from_arr(size_t n, char **name_table) {
    size_t max_str_len = 0;

    for(size_t i = 0; i < n; i++) {
        char *str_ptr = name_table[i];
        max_str_len = MAX(max_str_len, strlen(str_ptr));
    }

    return max_str_len;
}

void fprintf_str_block(FILE *stream, const size_t indent_sz, const size_t block_sz, const char *string) {
    assert(string != NULL);

    int str_len = (int) strlen(string);

    if ((int) block_sz < str_len) {
        debug("string crawls out of the block : string[%d] : '%s', block_sz : [%lu]", str_len, string, block_sz);
        str_len = (int) block_sz;
    }
    fprintf(stream, "%*.s", (int) indent_sz, "");
    fprintf(stream, "%*s%*.s", str_len, string, (int) block_sz - str_len, "");
}

void fprintf_indent_str(FILE *stream, const size_t indent_sz, const char fmt[], ...) {
    va_list args;
    fprintf(stream, "%*.s", (int) indent_sz, "");
    va_start(args, fmt);
    vfprintf(stream, fmt, args);

    va_end(args);
}

void fprintf_border(FILE* stream, const char bord_char, const size_t bord_sz, bool new_line) {
    for (size_t i = 0; i < bord_sz; i++) {
        fputc(bord_char, stream);
    }
    if (new_line) {
        fputc('\n', stream);
    }
}

void fprintf_title(FILE *stream, const char tittle[], const char bord_char, const size_t bord_sz) {
    assert(tittle != NULL);

    size_t tittle_sz = strlen(tittle);
    if (bord_sz < tittle_sz) {
        return;
    }
    size_t len = bord_sz - tittle_sz;
    fprintf_border(stream, bord_char, len / 2, false);
    fprintf_red(stream, "%s", tittle);
    fprintf_border(stream, bord_char, (len + 1) / 2, true);
}

void str_storage_t_dtor(str_storage_t *storage) { // Переписать алокатор на все типы void *
    if (storage == NULL) {
        debug("storage nullptr")
        return;
    }

    if (storage->prev_chunk == NULL) {
        if (storage->data == NULL) {
            debug("storage->data nullptr");
            return;
        }
        FREE(storage->data)
        FREE(storage)
        return;
    }

    str_storage_t_dtor((str_storage_t *) storage->prev_chunk); // FIXME: while prev chunk != NULL
    FREE(storage->data);
    FREE(storage);
}

char *get_new_str_ptr(str_storage_t **storage, const size_t len) {
    assert(storage != NULL);

    if (len + 1 >= (*storage)->chunk_size) {
        debug("string doesn't fit into chunk. chunk_size: {%lu}, string_len: {%lu}", (*storage)->chunk_size, len)
        return NULL;
    }

    if ((*storage)->idx + len + 1 >= (*storage)->chunk_size) {
        void *prev_chunk = *storage;

        *storage = str_storage_t_ctor((*storage)->chunk_size);
        (*storage)->prev_chunk = prev_chunk;

        return get_new_str_ptr(storage, len);
    }

    char *string_ptr = &((*storage)->data[(*storage)->idx]);
    (*storage)->idx += len + 1;

    return string_ptr;
}

int get_file_sz(const char *path) {
    struct stat buf = {};
    if (stat(path, &buf) != 0) {
        debug("stat error")
        return -1;
    }

    return (int) buf.st_size;
}

str_t read_text_from_file(const char *path) {
    int file_sz = get_file_sz(path);
    str_t str = {};

    if (file_sz < 0) {
        debug("file_sz invalid value : {%d}", file_sz)
        return {};
    }

    str.len = (size_t) file_sz;

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        debug("file '%s' open failed", path)
        return {};
    }

    str.str_ptr = (char *) calloc((size_t) file_sz + 1, sizeof(char)); // save one byte for '\0'

    size_t fread_cnt = fread(str.str_ptr, sizeof(char), (size_t) file_sz, file);
    assert(fread_cnt == (size_t) file_sz);

    if (fclose(file)) {
        debug("file '%s' close failed", path)
        return {};
    }
    return str;
}

bool write_text_to_file(const str_t *text, const char *path) {
    assert(text != NULL);
    assert(path != NULL);

    FILE *output_file_ptr = fopen(path, "wb");
    if (output_file_ptr == NULL) {
        debug("file '%s' open failed", path);
        return false;
    }

    fprintf(output_file_ptr, "%s", text->str_ptr);
    if (fclose(output_file_ptr)) {
        debug("file '%s' close failed", path)
        return false;
    }

    return true;
}

void remove_chars_from_text(str_t *text, const char sims[]) {
    size_t left = 0;
    size_t right = 0;

    while (right < text->len) {
        if (strchr(sims, text->str_ptr[right]) == NULL) {
            text->str_ptr[left] = text->str_ptr[right];
            left++;
            right++;
        } else {
            right++;
        }
    }

    size_t text_new_size = left;

    while (left < text->len) {
        text->str_ptr[left++] = '\0';
    }

    text->len = text_new_size;
}

void fprintf_bin(FILE *stream, int mask) {
    int start_mask = mask;

    const size_t bit_len = sizeof(mask) * 8;
    char res_str[bit_len + 1] = {};
    size_t res_idx = 0;

    char sims[] = "01";

    while (mask > 0) {
        res_str[res_idx++] = sims[mask % 2];
        mask /= 2;
    }
    while (res_idx < bit_len) {
        res_str[res_idx++] = '0';
    }

    fprintf(stream, "bin[%10d]: '%s'\n", start_mask, res_str);
}

void get_bit_str(int mask, char res_str[], const size_t n_bits) {
    assert(res_str != NULL);

    size_t res_idx = 0;

    size_t cnt_chars = 0;
    while (mask > 0 && res_idx < n_bits) {
        res_str[res_idx++] = (char) (mask % 2) + '0';
        mask /= 2;
    }
    while (res_idx < n_bits) {
        res_str[res_idx++] = '0';
    }
}
