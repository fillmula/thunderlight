#include <stdlib.h>
#include "response.h"


void Response_init(Response *self) {
    self->code = 200;
    HeaderMap_init(&(self->headers));
    self->body = NULL;
    self->body_len = 0;
    Buffer_init(&(self->buffer));
}

char *Response_get_header_bytes(Response *self, size_t *len) {
    Buffer_append(&(self->buffer), "HTTP/1.1 200 OK\r\n\r\n", 19);
    Buffer_append(&(self->buffer), "Server: ABCDEF\r\n", 16);
    Buffer_append(&(self->buffer), "Content-Length: 20\r\n", 16);
    *len = self->buffer.length;
    return self->buffer.loc;
}

char *Response_get_body_bytes(Response *self, size_t *len) {
    self->body = "{\"key\": \"value\"}";
    *len = self->body_len;
    return self->body;
}
