#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "response.h"
#include "space.h"


typedef struct {
    const char *message;
    const char *code;
    uint8_t length;
} StatusMessage;

StatusMessage messages[511] = {};

void StatusMessage_setup(void) {
    messages[100].message = "Continue";
    messages[100].code = "100";
    messages[100].length = 8;
    messages[200].message = "OK";
    messages[200].code = "200";
    messages[200].length = 2;
    messages[404].message = "Not Found";
    messages[404].code = "404";
    messages[404].length = 9;
}

void Response_init(Response *self) {
    self->code = 200;
    HeaderMap_init(&(self->headers));
    self->body = NULL;
    self->body_len = 0;
    Buffer_init(&(self->buffer));
}

void Response_set_version_info(Response *self, char *version, uint8_t version_len) {
    self->version = version;
    self->version_len = version_len;
}

char *Response_get_header_bytes(Response *self, size_t *len) {
    Buffer_append(&(self->buffer), self->version, self->version_len);
    Buffer_append(&(self->buffer), " ", 1);
    Buffer_append(&(self->buffer), messages[self->code].code, 3);
    Buffer_append(&(self->buffer), " ", 1);
    Buffer_append(&(self->buffer), messages[self->code].message, messages[self->code].length);
    Buffer_append(&(self->buffer), "\r\n\r\n", 4);
    char date[64];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    size_t date_len = strlen(date);
    Buffer_append(&(self->buffer), "Date: ", 6);
    Buffer_append(&(self->buffer), date, date_len);
    Buffer_append(&(self->buffer), "\r\n", 2);
    Buffer_append(&(self->buffer), "Server: Thunderlight 0.2.0\r\n", 28);
    // examine each header here
    for (size_t i = 0; i < self->headers.len; i++) {
        Buffer_append(&(self->buffer), self->headers.buffer[i].key, self->headers.buffer[i].key_len);
        Buffer_append(&(self->buffer), ": ", 2);
        Buffer_append(&(self->buffer), self->headers.buffer[i].value, self->headers.buffer[i].value_len);
        Buffer_append(&(self->buffer), "\r\n", 2);
    }
    Buffer_append(&(self->buffer), "Connection: close\r\n", 19);
    Buffer_append(&(self->buffer), "Content-Length: ", 16);
    char content_len[16];
    snprintf(content_len, 16, "%zu", self->body_len);
    Buffer_append(&(self->buffer), content_len, strlen(content_len));
    Buffer_append(&(self->buffer), "\r\n\r\n", 4);
    *len = self->buffer.length;
    return self->buffer.loc;
}

char *Response_get_body_bytes(Response *self, size_t *len) {
    *len = self->body_len;
    return self->body;
}

char *Response_repr(Response *self, char *head, uint8_t indent) {
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    strcat(buffer, "{\n");
    // code
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'code': ");
    char *code;
    asprintf(&code, "%hu", self->code);
    strcat(buffer, ",\n");
    // headers
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'headers': ");
    char *headers_repr = HeaderMap_repr(&self->headers, NULL, indent + 1);
    strcat(buffer, headers_repr);
    free(headers_repr);
    strcat(buffer, ",\n");
    // body
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'body': ");
    if (self->body_len == NULL) {
        strcat(buffer, "(empty)\n");
    } else {
        char bytes[32];
        sprintf(bytes, "(%ld bytes)\n", self->body_len);
        strcat(buffer, bytes);
    }
    add_space(buffer, indent * 4);
    strcat(buffer, "}");
    return buffer;
}
