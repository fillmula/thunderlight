#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include "request.h"

#define TOLOWER(x) (x) + 32

const char *CONTENT_LENGTH = "content-length";

RequestParsingState _Request_parse(Request *self);
RequestParsingState _parse_from_method(Request *self);
RequestParsingState _parse_from_path(Request *self);
RequestParsingState _parse_from_query(Request *self);
RequestParsingState _parse_from_version(Request *self);
RequestParsingState _parse_from_headers(Request *self);
RequestParsingState _parse_from_body(Request *self);

void Request_init(Request *self) {
    // buffer
    self->buffer_start = self->inline_buffer;
    self->buffer_end = self->inline_buffer;
    self->buffer_capacity = INLINE_BUFFER_SIZE;
    self->bytes_received = 0;
    // parser
    self->parser_pos = self->buffer_start;
    self->parser_location = RequestParserLocationMethod;
    self->exp_body_len = 0;
    // data
    self->method = self->buffer_start;
    self->method_len = 0;
    self->path = NULL;
    self->path_len = 0;
    self->path_offset = 0;
    self->query = NULL;
    self->query_len = 0;
    self->query_offset = 0;
    self->version = NULL;
    self->version_len = 0;
    self->version_offset = 0;
    self->parser_header_location = RequestParserHeaderLocationName;
    self->header_num = 1;
    self->body = NULL;
    self->body_len = 0;
    self->body_offset = 0;
}

void Request_dealloc(Request *self) {
    if (self->buffer_start != self->inline_buffer) {
        free(self->buffer_start);
    }
}

void Request_sync_parser_data_pointers(Request *self) {
    self->parser_pos = self->buffer_start + self->parser_offset;
    self->method = self->buffer_start;
    if (self->path == NULL) { return; }
    self->path = self->buffer_start + self->path_offset;
    if (self->query != NULL) {
        self->query = self->buffer_start + self->query_offset;
    }
    if (self->version == NULL) { return; }
    self->version = self->buffer_start + self->version_offset;
    // headers how to deal with?
    if (self->body == NULL) { return; }
    self->body = self->buffer_start + self->body_offset;
}

RequestParsingState Request_receive(Request *self, char *content, size_t len) {
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
    return _Request_parse(self);
}

RequestParsingState _Request_parse(Request *self) {
    switch (self->parser_location) {
    case RequestParserLocationDone:
        return RequestParsingStateDone;
    case RequestParserLocationBody:
        return _parse_from_body(self);
    case RequestParserLocationHeaders:
        return _parse_from_headers(self);
    case RequestParserLocationVersion:
        return _parse_from_version(self);
    case RequestParserLocationQuery:
        return _parse_from_query(self);
    case RequestParserLocationPath:
        return _parse_from_path(self);
    case RequestParserLocationMethod:
        return _parse_from_method(self);
    default:
        return RequestParsingStatePending;
    }
}

RequestParsingState _parse_from_method(Request *self) {
    while (self->parser_pos < self->buffer_end) {
        if (*(self->parser_pos) == ' ') {
            *(self->parser_pos) = '\0'; // replace ws with char * terminator
            self->parser_pos++;
            self->path = self->parser_pos;
            self->path_offset = self->parser_pos - self->buffer_start;
            self->parser_location = RequestParserLocationPath;
            goto next;
        }
        self->method_len++;
        self->parser_pos++;
    }
    goto ret;
next:
    return _parse_from_path(self);
ret:
    return RequestParsingStatePending;
}

RequestParsingState _parse_from_path(Request *self) {
    while (self->parser_pos < self->buffer_end) {
        if (*(self->parser_pos) == '?') {
            *(self->parser_pos) = '\0';
            self->parser_pos++;
            self->query = self->parser_pos;
            self->query_offset = self->parser_pos - self->buffer_start;
            self->parser_location = RequestParserLocationQuery;
            goto query;
        } else if (*(self->parser_pos) == ' ') {
            *(self->parser_pos) = '\0';
            self->parser_pos++;
            self->version = self->parser_pos;
            self->version_offset = self->parser_pos - self->buffer_start;
            self->parser_location = RequestParserLocationVersion;
            goto version;
        }
        self->path_len++;
        self->parser_pos++;
    }
    goto ret;
query:
    return _parse_from_query(self);
version:
    return _parse_from_version(self);
ret:
    return RequestParsingStatePending;
}

RequestParsingState _parse_from_query(Request *self) {
    while (self->parser_pos < self->buffer_end) {
        if (*(self->parser_pos) == ' ') {
            *(self->parser_pos) = '\0';
            self->parser_pos++;
            self->version = self->parser_pos;
            self->parser_location = RequestParserLocationVersion;
            goto version;
        }
        self->query_len++;
        self->parser_pos++;
    }
    goto ret;
version:
    return _parse_from_version(self);
ret:
    return RequestParsingStatePending;
}

RequestParsingState _parse_from_version(Request *self) {
    while (self->parser_pos < self->buffer_end) {
        if (*(self->parser_pos) == '\r') {
            *(self->parser_pos) = '\0';
            self->parser_pos += 2;
            self->parser_location = RequestParserLocationHeaders;
            self->headers[0].name = self->parser_pos;
            self->headers[0].name_len = 0;
            goto headers;
        }
        self->version_len++;
        self->parser_pos++;
    }
    goto ret;
headers:
        return _parse_from_headers(self);
ret:
    return RequestParsingStatePending;
}

RequestParsingState _parse_from_headers(Request *self) {
    while (self->parser_pos < self->buffer_end) {
        size_t index = self->header_num - 1;
        if ((self->parser_header_location == RequestParserHeaderLocationName) &&
            (*(self->parser_pos) == ':')) {
            *(self->parser_pos) = '\0';
            self->parser_pos += 2;
            self->parser_header_location = RequestParserHeaderLocationValue;
            self->headers[index].value = self->parser_pos;
            self->headers[index].value_len = 0;
        } else if ((self->parser_header_location == RequestParserHeaderLocationValue) &&
                   (*(self->parser_pos) == '\r')) {
            *(self->parser_pos) = '\0';
            self->parser_pos += 2;
            self->parser_header_location = RequestParserHeaderLocationLineBeginning;
            self->header_num++;
            self->headers[index + 1].name = self->parser_pos;
            self->headers[index + 1].name_len = 0;
        } else if (self->parser_header_location == RequestParserHeaderLocationLineBeginning) {
            if (strcmp(self->headers[index - 1].name, CONTENT_LENGTH) == 0) {
                self->exp_body_len = atoi(self->headers[index - 1].value);
            }
            if (*(self->parser_pos) == '\r') {
                self->parser_pos += 2;
                self->body = self->parser_pos;
                self->header_num--;
                self->parser_location = RequestParserLocationBody;
                goto body;
            } else if ((*(self->parser_pos) == ' ') || (*(self->parser_pos) == '\t')) {
                self->parser_header_location = RequestParserHeaderLocationContinuousLineBeginning;
                self->parser_pos++;
            } else {
                self->parser_header_location = RequestParserHeaderLocationName;
            }
        } else if (self->parser_header_location == RequestParserHeaderLocationContinuousLineBeginning) {
            if ((*(self->parser_pos) == ' ') || (*(self->parser_pos) == '\t')) {
                self->parser_pos++;
            } else {
                self->headers[index].name = NULL;
                self->headers[index].name_len = 0;
                self->headers[index].value = self->parser_pos;
                self->headers[index].value_len = 0;
                self->parser_pos++;
            }
        } else {
            if (self->parser_header_location == RequestParserHeaderLocationName) {
                if (*(self->parser_pos) <= 'Z' && *(self->parser_pos) >= 'A') {
                    *(self->parser_pos) = TOLOWER(*(self->parser_pos));
                }
                self->headers[index].name_len++;
            } else {
                self->headers[index].value_len++;
            }
            self->parser_pos++;
        }
    }
    goto ret;
body:
    return _parse_from_body(self);
ret:
    return RequestParsingStatePending;
}

RequestParsingState _parse_from_body(Request *self) {
    if ((self->buffer_end - self->body) >= self->exp_body_len) {
        return RequestParsingStateDone;
    } else {
        return RequestParsingStatePending;
    }
}

char *_Request_debug_headers(Request *self) {
    char *headers = malloc(255);
    headers[0] = '(';
    headers[1] = '\0';
    for (uint8_t i = 0; i < self->header_num; i++) {
        if (i != 0) {
            strcat(headers, ", ");
        }
        strcat(headers, "\"");
        strcat(headers, self->headers[i].name);
        strcat(headers, "\"");
        strcat(headers, ": ");
        strcat(headers, "\"");
        strcat(headers, self->headers[i].value);
        strcat(headers, "\"");
    }
    strcat(headers, ")");
    return headers;
}

void Request_print(Request *self) {
    char *headers = _Request_debug_headers(self);
    printf("Request(method: \"%s\", path: \"%s\", query: \"%s\", version: \"%s\", headers: %s, body: (%ld bytes))\n", self->method, self->path, self->query, self->version, _Request_debug_headers(self), self->buffer_end - self->body);
    free(headers);
}

void Request_debug_print(Request *self) {
    char *headers = _Request_debug_headers(self);
    printf("Request(method: \"%s\", path: \"%s\", query: \"%s\", version: \"%s\", headers: %s, body: (%ld bytes))\n", self->method, self->path, self->query, self->version, _Request_debug_headers(self), self->buffer_end - self->body);
    printf("exp %ld", self->exp_body_len);
    free(headers);
}
