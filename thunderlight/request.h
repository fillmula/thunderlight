#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define HEADERS_MAX 30
#define INLINE_BUFFER_SIZE 4096

typedef struct {
    char *name;
    size_t name_len;
    char *value;
    size_t value_len;
} Header;

typedef enum {
    RequestParserLocationMethod,
    RequestParserLocationPath,
    RequestParserLocationQuery,
    RequestParserLocationVersion,
    RequestParserLocationHeaders,
    RequestParserLocationBody,
    RequestParserLocationDone,
    RequestParserLocationError
} RequestParserLocation;

typedef enum {
    RequestParserHeaderLocationLineBeginning,
    RequestParserHeaderLocationContinuousLineBeginning,
    RequestParserHeaderLocationName,
    RequestParserHeaderLocationValue
} RequestParserHeaderLocation;

typedef enum {
    RequestParsingStatePending,
    RequestParsingStateDone,
    RequestParsingStateError
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
    char *query;
    size_t query_len;
    size_t query_offset;
    char *version;
    size_t version_len;
    size_t version_offset;
    Header headers[HEADERS_MAX];
    RequestParserHeaderLocation parser_header_location;
    uint8_t header_num;
    char *body;
    size_t body_len;
    size_t body_offset;
} Request;

void Request_init(Request *self);

void Request_dealloc(Request *self);

RequestParsingState Request_receive(Request *self, char *content, size_t len);

char *Request_method(Request *self);

char *Request_path(Request *self);

char *Request_query(Request *self);

char *Request_version(Request *self);

Header *Request_headers(Request *self);

size_t Request_content_len(Request *self);

void Request_print(Request *self);

void Request_debug_print(Request *self);

#ifdef __cplusplus
}
#endif
