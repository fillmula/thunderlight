#include <stddef.h>


#define INLINE_BUFFER_SIZE 4096


typedef struct {
    char *content;
    size_t received;
    char *end;
    size_t capacity;
    char inline_content[INLINE_BUFFER_SIZE];
} Buffer;

void Buffer_init(Buffer *self);

void Buffer_append(Buffer *self, char *content, size_t len);
