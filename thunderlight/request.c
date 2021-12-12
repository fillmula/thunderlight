#include <stdbool.h>
#include "request.h"


void Request_init(Request *self, Buffer *buffer) {
    self->buffer = buffer;
    self->offset = 0;
    self->method = self->buffer->content;
    self->method_len = 0;
    self->state = METHOD;
}

void _parse_from_method(Request *self);
void _parse_from_path(Request *self);
void _parse_from_qs(Request *self);
void _parse_from_version(Request *self);
void _parse_from_headers(Request *self);
void _parse_from_body(Request *self);

void Request_parse(Request *self) {
    switch (self->state) {
    case DONE:
        break;
    case BODY:
        _parse_from_body(self);
        break;
    case HEADER:
        _parse_from_headers(self);
        break;
    case VERSION:
        _parse_from_version(self);
        break;
    case QS:
        _parse_from_qs(self);
        break;
    case PATH:
        _parse_from_path(self);
        break;
    case METHOD:
        _parse_from_method(self);
        break;
    default:
        break;
    }
}

void _parse_from_method(Request *self) {
    char *pos = self->buffer->content + self->offset;
    char *buffer_end = self->buffer->content + self->buffer->received;
    bool cont = false;
    while (pos < buffer_end) {
        if (*pos == ' ') {
            *pos = '\0';
            pos++;
            self->state = PATH;
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
