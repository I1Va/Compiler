#ifndef TRANSLATOR_STRUCTS_H
#define TRANSLATOR_STRUCTS_H

#include <stdio.h>


struct var_t {
    int deep;
    int type;
    int name_id;
    char *name;
    int loc_addr;
};

const var_t POISON_VAR = {-1, -1, -1, NULL};


#endif // TRANSLATOR_STRUCTS_H