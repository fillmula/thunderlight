#include <string.h>
#include <sys/param.h>

#include "buffer.h"

void Buffer_init(Buffer *self) {
    self->content = self->inline_content;
    self->capacity = INLINE_BUFFER_SIZE;
    self->received = 0;
    self->end = self->content;
}

void Buffer_append(Buffer *self, char *content, size_t len) {
    if (len > self->capacity - self->received) {
        // copy or reallocate a new buffer in memory heap if needed
        self->capacity = MAX(self->capacity * 2, self->received + len);
        if (self->content == self->inline_content) {
            self->content = malloc(self->capacity);
            memcpy(self->content, self->inline_content, self->received);
        } else {
            self->content = realloc(self->content, self->capacity);
        }
    }
    // append new content to the buffer
    memcpy(self->content + self->received, content, len);
    self->received += len;
    self->end = self->content + self->received;
}
