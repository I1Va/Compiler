#ifndef ELF_TRANSLATOR_GENERAL_H
#define ELF_TRANSLATOR_GENERAL_H

#include <string.h>
#include <stdio.h>


#define GRN "\e[0;32m"
#define WHT "\e[0;20m"
#define RED "\e[0;31m"
#define YEL "\e[0;33m"

#define HTML_GRN "<font color=\"green\">"
#define HTML_BLACK "<font color=\"black\">"
#define HTML_RED "<font color=\"red\">"
#define HTML_YEL "<font color=\"yellow\">"

#define printf_red(str_, ...) printf(RED str_ WHT, ##__VA_ARGS__)
#define printf_wht(str_, ...) printf(WHT str_ WHT, ##__VA_ARGS__)
#define printf_grn(str_, ...) printf(GRN str_ WHT, ##__VA_ARGS__)
#define printf_yel(str_, ...) printf(YEL str_ WHT, ##__VA_ARGS__)

#define fprintf_red(stream, str_, ...) fprintf(stream, RED str_ WHT, ##__VA_ARGS__)
#define fprintf_wht(stream, str_, ...) fprintf(stream, WHT str_ WHT, ##__VA_ARGS__)
#define fprintf_grn(stream, str_, ...) fprintf(stream, GRN str_ WHT, ##__VA_ARGS__)
#define fprintf_yel(stream, str_, ...) fprintf(stream, YEL str_ WHT, ##__VA_ARGS__)

#define fprintf_html_red(stream, str_, ...) fprintf(stream, HTML_RED str_, ##__VA_ARGS__)
#define fprintf_html_grn(stream, str_, ...) fprintf(stream, HTML_GRN str_, ##__VA_ARGS__)
#define fprintf_html_black(stream, str_, ...) fprintf(stream, HTML_BLACK str_, ##__VA_ARGS__)
#define fprintf_html_yel(stream, str_, ...) fprintf(stream, HTML_YEL str_, ##__VA_ARGS__)

#define CLEAR_MEMORY(mark) goto mark;

#define FREE(ptr)              \
    do {                       \
        free(ptr); ptr = NULL; \
    } while(0);

#ifndef NDEBUG
    #define debug(str_, ...) fprintf_red(stderr, "{%s} [%s: %d]: descr{" str_ "}\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__);
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define debug(str_, ...) ;
    #define ON_DEBUG(...)
#endif // NDEBUG


const unsigned char GAP_BYTE = 0x00;
void write_bytes_to_field(unsigned char field[], const size_t field_sz, ...);


#endif // ELF_TRANSLATOR_GENERAL_H