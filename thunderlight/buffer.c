#include <stdlib.h>
#include <sys/param.h>
#include <string.h>
#include "buffer.h"


void Buffer_init(Buffer *buffer) {
    buffer->length = 0;
    buffer->loc = malloc(BUFFER_INITIAL_SIZE);
    buffer->pos = buffer->loc;
    buffer->capacity = BUFFER_INITIAL_SIZE;
}

void Buffer_append(Buffer *buffer, char *content, size_t len) {
    if (len + buffer->length > buffer->capacity) {
        buffer->loc = realloc(buffer->loc, MAX(len + buffer->length, buffer->capacity + BUFFER_INITIAL_SIZE));
        buffer->pos = buffer->loc + buffer->length;
    }
    memcpy(buffer->pos, content, len);
    buffer->length += len;
    buffer->pos += len;
}

void Buffer_dealloc(Buffer *buffer) {
    free(buffer->loc);
}
