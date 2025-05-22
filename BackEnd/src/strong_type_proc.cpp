
#include <stdio.h>
#include <stdlib.h>

#include "FrontEnd.h"
#include "general.h"
#include "lang_logger.h"


static char bufer[BUFSIZ] = {};

void raise_typization_error(const lexer_token_t token_type_1, const lexer_token_t token_type_2, const char file_name[], const char function[], const int line) {
    int offset = 0;
    offset += token_write(bufer + offset, BUFSIZ, token_type_1);
    offset += snprintf(bufer + offset,  BUFSIZ, "not compatible with ");
    offset += token_write(bufer + offset, BUFSIZ, token_type_2);

    fprintf_red(stderr, "{%s} [%s: %d]: translator_typization_error: %s \n", file_name, function, line, bufer);
    abort();
}
