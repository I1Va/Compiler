#ifndef STRONG_TYPE_PROC_H
#define STRONG_TYPE_PROC_H

#define TYPIZATION_ERROR_RAISE(type1, type2) \
    fprintf_red(stderr, "{%s} [%s: %d]: translator_typization_error: %\n", __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); abort();




#endif // STRONG_TYPE_PROC_H