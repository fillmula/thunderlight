#include <stdlib.h>
#include <string.h>
#include "response_headers.h"
#include "hash.h"


HeaderMap *HeaderMap_new(void) {
    HeaderMap *header_map = malloc(sizeof(HeaderMap));
    return header_map;
}

void HeaderMap_init(HeaderMap *self) {
    self->buffer = self->inline_buffer;
    self->capacity = HEADER_MAP_INITIAL_SIZE;
    self->len = 0;
}

void HeaderMap_dealloc(HeaderMap *self) {
    for (size_t i = 0; i < self->len; i++) {
        free(self->buffer[i].value);
    }
    if (self->buffer != self->inline_buffer) {
        free(self->buffer);
    }
    free(self);
}

void HeaderMap_set(HeaderMap *self, char *key, void *value, size_t value_len) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    HeaderItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    if (pos != NULL) {
        pos->key_hash = hash;
        free(pos->value);
        char *copied_value = malloc(value_len);
        memcpy(copied_value, value, value_len);
        pos->value = copied_value;
    } else {
        if (self->len == self->capacity) {
            size_t copy_size = self->capacity * sizeof(HeaderItem);
            self->capacity = self->capacity + HEADER_MAP_INITIAL_SIZE;
            if (self->buffer == self->inline_buffer) {
                self->buffer = malloc(self->capacity * sizeof(HeaderItem));
                memcpy(self->buffer, self->inline_buffer, copy_size);
            } else {
                self->buffer = realloc(self->buffer, self->capacity * sizeof(HeaderItem));
            }
        }
        size_t index = self->len;
        self->len++;
        self->buffer[index].key_hash = hash;
        char *copied_value = malloc(value_len);
        memcpy(copied_value, value, value_len);
        self->buffer[index].value = copied_value;
    }
}

char *HeaderMap_get(HeaderMap *self, char *key) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            return self->buffer[i].value;
        }
    }
    return NULL;
}
