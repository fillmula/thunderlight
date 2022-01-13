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
    messages[101].message = "Switching Protocols";
    messages[101].code = "101";
    messages[101].length = 19;
    messages[102].message = "Processing";
    messages[102].code = "102";
    messages[102].length = 10;
    messages[103].message = "Early Hints";
    messages[103].code = "103";
    messages[103].length = 11;
    messages[200].message = "OK";
    messages[200].code = "200";
    messages[200].length = 2;
    messages[201].message = "Created";
    messages[201].code = "201";
    messages[201].length = 7;
    messages[202].message = "Accepted";
    messages[202].code = "202";
    messages[202].length = 8;
    messages[203].message = "Non-Authoritative Information";
    messages[203].code = "203";
    messages[203].length = 29;
    messages[204].message = "No Content";
    messages[204].code = "204";
    messages[204].length = 10;
    messages[205].message = "Reset Content";
    messages[205].code = "205";
    messages[205].length = 13;
    messages[206].message = "Partial Content";
    messages[206].code = "206";
    messages[206].length = 15;
    messages[207].message = "Multi-Status";
    messages[207].code = "207";
    messages[207].length = 12;
    messages[208].message = "Already Reported";
    messages[208].code = "208";
    messages[208].length = 16;
    messages[226].message = "IM Used";
    messages[226].code = "226";
    messages[226].length = 7;
    messages[300].message = "Multiple Choice";
    messages[300].code = "300";
    messages[300].length = 15;
    messages[301].message = "Moved Permanently";
    messages[301].code = "301";
    messages[301].length = 17;
    messages[302].message = "Found";
    messages[302].code = "302";
    messages[302].length = 5;
    messages[303].message = "See Other";
    messages[303].code = "303";
    messages[303].length = 9;
    messages[304].message = "Not Modified";
    messages[304].code = "304";
    messages[304].length = 12;
    messages[305].message = "Use Proxy";
    messages[305].code = "305";
    messages[305].length = 9;
    messages[306].message = "unused";
    messages[306].code = "306";
    messages[306].length = 6;
    messages[307].message = "Temporary Redirect";
    messages[307].code = "307";
    messages[307].length = 18;
    messages[308].message = "Permanent Redirect";
    messages[308].code = "308";
    messages[308].length = 18;
    messages[400].message = "Bad Request";
    messages[400].code = "400";
    messages[400].length = 11;
    messages[401].message = "Unauthorized";
    messages[401].code = "401";
    messages[401].length = 12;
    messages[402].message = "Payment Required";
    messages[402].code = "402";
    messages[402].length = 16;
    messages[403].message = "Forbidden";
    messages[403].code = "403";
    messages[403].length = 9;
    messages[404].message = "Not Found";
    messages[404].code = "404";
    messages[404].length = 9;
    messages[405].message = "Method Not Allowed";
    messages[405].code = "405";
    messages[405].length = 18;
    messages[406].message = "Not Acceptable";
    messages[406].code = "406";
    messages[406].length = 14;
    messages[407].message = "Proxy Authentication Required";
    messages[407].code = "407";
    messages[407].length = 29;
    messages[408].message = "Request Timeout";
    messages[408].code = "408";
    messages[408].length = 15;
    messages[409].message = "Conflict";
    messages[409].code = "409";
    messages[409].length = 8;
    messages[410].message = "Gone";
    messages[410].code = "410";
    messages[410].length = 4;
    messages[411].message = "Length Required";
    messages[411].code = "411";
    messages[411].length = 15;
    messages[412].message = "Precondition Failed";
    messages[412].code = "412";
    messages[412].length = 19;
    messages[413].message = "Payload Too Large";
    messages[413].code = "413";
    messages[413].length = 17;
    messages[414].message = "URI Too Long";
    messages[414].code = "414";
    messages[414].length = 12;
    messages[415].message = "Unsupported Media Type";
    messages[415].code = "415";
    messages[415].length = 22;
    messages[416].message = "Range Not Satisfiable";
    messages[416].code = "416";
    messages[416].length = 21;
    messages[417].message = "Expectation Failed";
    messages[417].code = "417";
    messages[417].length = 18;
    messages[418].message = "I'm a teapot";
    messages[418].code = "418";
    messages[418].length = 12;
    messages[421].message = "Misdirected Request";
    messages[421].code = "421";
    messages[421].length = 19;
    messages[422].message = "Unprocessable Entity";
    messages[422].code = "422";
    messages[422].length = 20;
    messages[423].message = "Locked";
    messages[423].code = "423";
    messages[423].length = 6;
    messages[424].message = "Failed Dependency";
    messages[424].code = "424";
    messages[424].length = 17;
    messages[425].message = "Too Early";
    messages[425].code = "425";
    messages[425].length = 9;
    messages[426].message = "Upgrade Required";
    messages[426].code = "426";
    messages[426].length = 16;
    messages[428].message = "Precondition Required";
    messages[428].code = "428";
    messages[428].length = 21;
    messages[429].message = "Too Many Requests";
    messages[429].code = "429";
    messages[429].length = 17;
    messages[431].message = "Request Header Fields Too Large";
    messages[431].code = "431";
    messages[431].length = 31;
    messages[451].message = "Unavailable For Legal Reasons";
    messages[451].code = "451";
    messages[451].length = 29;
    messages[500].message = "Internal Server Error";
    messages[500].code = "500";
    messages[500].length = 21;
    messages[501].message = "Not Implemented";
    messages[501].code = "501";
    messages[501].length = 15;
    messages[502].message = "Bad Gateway";
    messages[502].code = "502";
    messages[502].length = 11;
    messages[503].message = "Service Unavailable";
    messages[503].code = "503";
    messages[503].length = 19;
    messages[504].message = "Gateway Timeout";
    messages[504].code = "504";
    messages[504].length = 15;
    messages[505].message = "HTTP Version Not Supported";
    messages[505].code = "505";
    messages[505].length = 26;
    messages[506].message = "Variant Also Negotiates";
    messages[506].code = "506";
    messages[506].length = 23;
    messages[507].message = "Insufficient Storage";
    messages[507].code = "507";
    messages[507].length = 20;
    messages[508].message = "Loop Detected";
    messages[508].code = "508";
    messages[508].length = 13;
    messages[510].message = "Not Extended";
    messages[510].code = "510";
    messages[510].length = 12;
    messages[511].message = "Network Authentication Required";
    messages[511].code = "511";
    messages[511].length = 31;
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
    Buffer_append(&(self->buffer), "\r\n", 2);
    char date[64];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    size_t date_len = strlen(date);
    Buffer_append(&(self->buffer), "Date: ", 6);
    Buffer_append(&(self->buffer), date, date_len);
    Buffer_append(&(self->buffer), "\r\n", 2);
    Buffer_append(&(self->buffer), "Server: Thunderlight/0.2.0\r\n", 28);
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
