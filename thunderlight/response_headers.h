#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define HEADER_MAP_INITIAL_SIZE 16

typedef struct {
    uint32_t key_hash;
    char *value;
} HeaderItem;

typedef struct {
    HeaderItem *buffer;
    HeaderItem inline_buffer[HEADER_MAP_INITIAL_SIZE];
    size_t len;
    size_t capacity;
} HeaderMap;

HeaderMap *HeaderMap_new(void);

void HeaderMap_init(HeaderMap *self);

void HeaderMap_dealloc(HeaderMap *self);

void HeaderMap_set(HeaderMap *self, char *key, void *value, size_t value_len);

char *HeaderMap_get(HeaderMap *self, char *key);

#ifdef __cplusplus
}
#endif
