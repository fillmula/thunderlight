#include <string.h>
#include <stdio.h>
#include "mresult.h"
#include "space.h"


MatchResult *MatchResult_new(size_t len) {
    size_t size = sizeof(MatchResult) + sizeof(MatchResultItem) * len;
    MatchResult *self = (MatchResult *)malloc(size);
    self->len = len;
    self->items = self + sizeof(MatchResult);
    return self;
}

void MatchResult_set(MatchResult *self, size_t index, char *key, char *value) {
    self->items[index].key = key;
    self->items[index].value = value;
}

char *MatchResult_get(MatchResult *self, char *key) {
    for (size_t i = 0; i < self->len; i++) {
        if (strcmp(self->items[i].key, key) == 0) {
            return self->items[i].value;
        }
    }
    return NULL;
}

void MatchResult_dealloc(MatchResult *self) {
    free(self);
}

char *MatchResult_repr(MatchResult *self, char *head, uint8_t indent) {
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    if (self->len == 0) {
        strcat(buffer, "{}");
        return buffer;
    }
    strcat(buffer, "{\n");
    for (size_t i = 0; i < self->len; i++) {
        if (i != 0) {
            strcat(buffer, ",\n");
        }
        add_space(buffer, (indent + 1) * 4);
        strcat(buffer, "'");
        strcat(buffer, self->items[i].key);
        strcat(buffer, "'");
        strcat(buffer, ": ");
        strcat(buffer, "'");
        strcat(buffer, self->items[i].value);
        strcat(buffer, "'");
    }
    strcat(buffer, "\n");
    add_space(buffer, indent * 4);
    strcat(buffer, "}");
    return buffer;
}
