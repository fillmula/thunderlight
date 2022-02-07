#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#define BUFFER_INITIAL_SIZE 1024

typedef struct {
    char *loc;
    char *pos;
    size_t capacity;
    size_t length;
} Buffer;

void Buffer_init(Buffer *buffer);

void Buffer_append(Buffer *buffer, char *content, size_t len);

void Buffer_dealloc(Buffer *buffer);

#ifdef __cplusplus
}
#endif
