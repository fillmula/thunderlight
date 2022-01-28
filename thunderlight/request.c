#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include "request.h"
#include "space.h"


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
    self->parser_header_location = RequestParserHeaderLocationName;
    self->exp_body_len = 0;
    // data
    self->method = self->buffer_start;
    self->method_len = 0;
    self->path = NULL;
    self->path_len = 0;
    self->query = NULL;
    self->query_len = 0;
    self->version = NULL;
    self->version_len = 0;
    self->header_num = 1;
    self->body = NULL;
    self->body_len = 0;
    // url match result
    self->mresult = NULL;
}

void Request_dealloc(Request *self) {
    if (self->buffer_start != self->inline_buffer) {
        free(self->buffer_start);
    }
    if (self->mresult != NULL) {
        free(self->mresult);
    }
}

RequestParsingState Request_receive(Request *self, char *content, size_t len) {
    if (len > self->buffer_capacity - self->bytes_received) {
        // calculate pointer offsets
        char *previous_buffer_start = self->buffer_start;
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
        // reset pointers
        size_t offset = self->buffer_start - previous_buffer_start;
        self->method = self->buffer_start;
        self->path = (self->path == NULL) ? NULL : self->path + offset;
        self->query = (self->query == NULL) ? NULL : self->query + offset;
        self->version = (self->version == NULL) ? NULL : self->version + offset;
        for (size_t i = 0; i < self->header_num; i++) {
            self->headers[i].name += offset;
            self->headers[i].value += offset;
        }
        self->body = (self->body == NULL) ? NULL : self->body + offset;
        // reset parser state
        self->parser_pos += offset;
    }
    // append new content to the buffer
    memcpy(self->buffer_end, content, len);
    self->bytes_received += len;
    self->buffer_end += len;
    RequestParsingState state = _Request_parse(self);
    return state;
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
            self->parser_location = RequestParserLocationQuery;
            goto query;
        } else if (*(self->parser_pos) == ' ') {
            *(self->parser_pos) = '\0';
            self->parser_pos++;
            self->version = self->parser_pos;
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

char *Request_repr(Request *self, char *head, uint8_t indent) {
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    strcat(buffer, "{\n");
    // method
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'method': '");
    strcat(buffer, self->method);
    strcat(buffer, "',\n");
    // path
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'path': '");
    strcat(buffer, self->path);
    strcat(buffer, "',\n");
    // query
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'query': ");
    if (self->query == NULL) {
        strcat(buffer, "None,\n");
    } else {
        strcat(buffer, "'");
        strcat(buffer, self->query);
        strcat(buffer, "',\n");
    }
    // version
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'version': '");
    strcat(buffer, self->version);
    strcat(buffer, "',\n");
    // headers
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'headers': ");
    char *headers_repr = Request_headers_repr(self, NULL, indent + 1);
    strcat(buffer, headers_repr);
    free(headers_repr);
    strcat(buffer, ",\n");
    // body
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'body': ");
    if (self->body_len == 0) {
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

char *Request_headers_repr(Request *self, char *head, uint8_t indent) {
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    if (self->header_num == 0) {
        strcat(buffer, "{}");
        return buffer;
    }
    strcat(buffer, "{\n");
    for (uint8_t i = 0; i < self->header_num; i++) {
        if (i != 0) {
            strcat(buffer, ",\n");
        }
        add_space(buffer, (indent + 1) * 4);
        strcat(buffer, "'");
        strcat(buffer, self->headers[i].name);
        strcat(buffer, "'");
        strcat(buffer, ": ");
        strcat(buffer, "'");
        strcat(buffer, self->headers[i].value);
        strcat(buffer, "'");
    }
    strcat(buffer, "\n");
    add_space(buffer, indent * 4);
    strcat(buffer, "}");
    return buffer;
}

void Request_print(Request *self) {
    char *repr = Request_repr(self, "Request", 0);
    printf(repr);
    free(repr);
}
