#include <stddef.h>


typedef struct {
    char *name;
    size_t name_len;
    char *value;
    size_t value_len;
} Header;
