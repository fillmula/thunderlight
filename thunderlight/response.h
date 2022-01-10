#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "buffer.h"
#include "response_headers.h"


#define RESPONSE_INITIAL_BUFFER_SIZE 1024

typedef struct {
    uint16_t code;
    HeaderMap headers;
    char *body;
    size_t body_len;
    Buffer buffer;
} Response;

void Response_init(Response *self);

char *Response_get_header_bytes(Response *self, size_t *len);

char *Response_get_body_bytes(Response *self, size_t *len);

#ifdef __cplusplus
}
#endif
