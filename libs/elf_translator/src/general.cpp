#include <assert.h>
#include <stdarg.h>
#include <string.h>


void write_bytes_to_field(unsigned char field[], const size_t field_sz, ...) {
    assert(field);

    va_list factor = {};
    va_start(factor, field_sz);

    for(size_t i = 0; i < field_sz; i++) {
        unsigned char cur_byte = va_arg(factor, int);
        field[i] = cur_byte;
    }

    va_end(factor);
}
