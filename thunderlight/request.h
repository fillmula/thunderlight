#pragma once

#include "buffer.h"
#include "header.h"


#define HEADERS_MAX 30
#define INLINE_BUFFER_SIZE 4096


typedef enum {
    RequestParserLocationMethod,
    RequestParserLocationPath,
    RequestParserLocationQS,
    RequestParserLocationVersion,
    RequestParserLocationHeaders,
    RequestParserLocationBody,
    RequestParserLocationDone,
    RequestParserLocationError
} RequestParserLocation;


typedef enum {
    RequestParsingState,

} RequestParsingState;


typedef struct {
    // buffer
    char *buffer_start;
    char *buffer_end;
    size_t buffer_capacity;
    char inline_buffer[INLINE_BUFFER_SIZE];
    size_t bytes_received;
    // parser
    size_t parser_offset;
    char *parser_pos;
    RequestParserLocation parser_location;
    size_t exp_body_len;
    // request
    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    size_t path_offset;
    char *qs;
    size_t qs_len;
    size_t qs_offset;
    char *version;
    size_t version_len;
    size_t version_offset;
    Header headers[HEADERS_MAX];
    size_t header_num;
    char *body;
    size_t body_len;
    size_t body_offset;
} Request;

void Request_init(Request *self);

void Request_deinit(Request *self);

void Request_receive(Request *self, char *content, size_t len);

char *Request_method(Request *self);

char *Request_path(Request *self);

char *Request_qs(Request *self);

char *Request_version(Request *self);

Header *Request_headers(Request *self);

size_t Request_content_len(Request *self);
