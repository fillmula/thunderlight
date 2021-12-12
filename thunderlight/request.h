#include "buffer.h"
#include "header.h"


typedef enum {
    METHOD,
    PATH,
    QS,
    VERSION,
    HEADER,
    BODY,
    DONE
} ParsingState;

//POST /categories HTTP/1.1\r\nContent-Type: text/plain\r\n
typedef struct {
    Buffer *buffer;
    size_t offset;
    char *method;
    size_t method_len;
    char *path;
    size_t path_len;
    char *qs;
    size_t qs_len;
    char *version;
    size_t version_len;
    Header headers[30];
    size_t header_num;
    size_t exp_len;
    char *body;
    ParsingState state;
} Request;

void Request_init(Request *self, Buffer *buffer);

void Request_parse(Request *self);
