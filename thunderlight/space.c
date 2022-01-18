#include <string.h>
#include "space.h"

void add_space(char *buffer, uint8_t num) {
    char *pos = buffer + strlen(buffer);
    memset(pos, ' ', num);
    *(pos + num) = '\0';
}
