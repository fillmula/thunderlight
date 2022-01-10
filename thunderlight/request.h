#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define HEADERS_MAX 32
#define INLINE_BUFFER_SIZE 4096

typedef struct {
    char *name;
    size_t name_len;
    char *value;
    size_t value_len;
} RequestHeader;

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
    char *parser_pos;
    RequestParserLocation parser_location;
    RequestParserHeaderLocation parser_header_location;
    size_t exp_body_len;
    // request
    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    char *query;
    size_t query_len;
    char *version;
    size_t version_len;
    RequestHeader headers[HEADERS_MAX];
    uint8_t header_num;
    char *body;
    size_t body_len;
} Request;

void Request_init(Request *self);

void Request_dealloc(Request *self);

RequestParsingState Request_receive(Request *self, char *content, size_t len);

#define Request_method(request) (request)->method

#define Request_path(request) (request)->path

#define Request_query(request) (request)->query

#define Request_version(request) (request)->version

RequestHeader *Request_headers(Request *self);

size_t Request_content_len(Request *self);

void Request_print(Request *self);

void Request_debug_print(Request *self);

#ifdef __cplusplus
}
#endif
