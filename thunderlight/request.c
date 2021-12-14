#include <stdbool.h>
#include <string.h>
#include "request.h"


void Request_init(Request *self) {
    // buffer
    self->buffer_start = self->inline_buffer;
    self->buffer_end = self->inline_buffer;
    self->buffer_capacity = INLINE_BUFFER_SIZE;
    self->bytes_received = 0;
    // parser
    self->parser_offset = 0;
    self->parser_pos = self->buffer_start;
    self->parser_location = RequestParserLocationMethod;
    self->exp_body_len = 0;
    // data
    self->method = self->buffer_start;
    self->method_len = 0;
    self->path = NULL;
    self->path_len = 0;
    self->path_offset = 0;
    self->qs = NULL;
    self->qs_len = 0;
    self->qs_offset = 0;
    self->version = NULL;
    self->version_len = 0;
    self->version_offset = 0;
    // self->header how to deal with this?
    self->header_num = 0;
    self->body = NULL;
    self->body_len = 0;
    self->body_offset = 0;
}

void Request_deinit(Request *self) {
    if (self->buffer_start != self->inline_buffer) {
        free(self->buffer_start);
    }
}

void Request_sync_parser_data_pointers(Request *self) {
    self->parser_pos = self->buffer_start + self->parser_offset;
    self->method = self->buffer_start;
    if (self->path == NULL) { return; }
    self->path = self->buffer_start + self->path_offset;
    if (self->qs != NULL) {
        self->qs = self->buffer_start + self->qs_offset;
    }
    if (self->version == NULL) { return; }
    self->version = self->buffer_start + self->version_offset;
    // headers how to deal with?
    if (self->body == NULL) { return; }
    self->body = self->buffer_start + self->body_offset;
}

void Request_receive(Request *self, char *content, size_t len) {
    if (len > self->buffer_capacity - self->bytes_received) {
        // copy or reallocate a new buffer in memory heap if needed
        self->buffer_capacity = MAX(self->buffer_capacity * 2, self->bytes_received + len);
        if (self->buffer_start == self->inline_buffer) {
            self->buffer_start = malloc(self->buffer_capacity);
            self->buffer_end = self->buffer_start + self->bytes_received;
            memcpy(self->buffer_start, self->inline_buffer, self->bytes_received);
        } else {
            self->buffer_start = realloc(self->buffer_start, self->buffer_capacity);
            self->buffer_end = self->buffer_start + self->bytes_received;
        }
        // reset parser state

    }
    // append new content to the buffer
    memcpy(self->buffer_end, content, len);
    self->bytes_received += len;
    self->buffer_end += len;
}



void _parse_from_method(Request *self);
void _parse_from_path(Request *self);
void _parse_from_qs(Request *self);
void _parse_from_version(Request *self);
void _parse_from_headers(Request *self);
void _parse_from_body(Request *self);

void Request_parse(Request *self) {
    switch (self->state) {
    case ParsingStateDone:
        break;
    case ParsingStateBody:
        _parse_from_body(self);
        break;
    case ParsingStateHeaders:
        _parse_from_headers(self);
        break;
    case ParsingStateVersion:
        _parse_from_version(self);
        break;
    case ParsingStateQS:
        _parse_from_qs(self);
        break;
    case ParsingStatePath:
        _parse_from_path(self);
        break;
    case ParsingStateMethod:
        _parse_from_method(self);
        break;
    default:
        break;
    }
}

void _parse_from_method(Request *self) {
    GOTO_POS;
    char *buffer_end = self->buffer->end;
    bool cont = false;
    while (pos < buffer_end) {
        if (*pos == ' ') {
            *pos = '\0';
            pos++;
            self->state = ParsingStatePath;
            self->offset = pos - self->buffer->content;
            self->path = pos;
            self->path_len = 0;
            self->qs = NULL;
            self->qs_len = 0;
            cont = true;
            break;
        }
        self->method_len++;
        pos++;
    }
    if (cont) {
        _parse_from_path(self);
    }
}

void _parse_from_path(Request *self) {
    char *pos = self->buffer->content + self->offset;
    char *buffer_end = self->buffer->content + self->buffer->received;
    bool cont = false;
    bool goto_qs = false;
    while (pos < buffer_end) {
        if (*pos == '?') {
            *pos = '\0';
            pos++;
            self->qs = pos;
            self->qs_len = 0;
            self->state = QS;
            self->offset = pos - self->buffer->content;
            goto_qs = true;
            cont = true;
            break;
        } else if (*pos == ' ') {
            *pos = '\0';
            pos++;
            self->version = pos;
            self->version_len = 0;
            self->state = VERSION;
            self->offset = pos - self->buffer->content;
            cont = true;
            break;
        }
        self->path_len++;
        pos++;
    }
    if (cont) {
        if (goto_qs) {
            _parse_from_qs(self);
        } else {
            _parse_from_version(self);
        }
    }
}

void _parse_from_qs(Request *self) {
    char *pos = self->buffer->content + self->offset;
    char *buffer_end = self->buffer->content + self->buffer->received;
    bool cont = false;
    while (pos < buffer_end) {
        if (*pos == ' ') {
            *pos = '\0';
            pos++;
            self->version = pos;
            self->version_len = 0;
            self->state = VERSION;
            self->offset = pos - self->buffer->content;
            cont = true;
            break;
        }
        self->qs_len++;
        pos++;
    }
    if (cont) {
        _parse_from_version(self);
    }
}

void _parse_from_version(Request *self) {
    char *pos = self->buffer->content + self->offset;
    char *buffer_end = self->buffer->content + self->buffer->received;
    bool cont = false;
    while (pos < buffer_end) {
        if (*pos == '\r') {
            *pos = '\0';
            pos += 2;
            self->header_num = 0;
            self->state = HEADER;
            self->offset = pos - self->buffer->content;
            cont = true;
            break;
        }
        self->version_len++;
        pos++;
    }
    if (cont) {
        _parse_from_headers(self);
    }
}

void _parse_from_headers(Request *self) {

}

void _parse_from_body(Request *self) {

}
