#ifndef DOT_LOG_H
#define DOT_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "graphviz_funcs.h"

void dot_code_t_dump(FILE *log_file, dot_code_t *dot_code);

#endif // DOT_LOG_H
