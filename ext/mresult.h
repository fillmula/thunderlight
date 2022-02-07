#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>


typedef struct {
    char *key;
    char *value;
} MatchResultItem;

typedef struct {
    size_t len;
    MatchResultItem *items;
} MatchResult;

MatchResult *MatchResult_new(size_t len);

void MatchResult_set(MatchResult *self, size_t index, char *key, char *value);

char *MatchResult_get(MatchResult *self, char *key);

void MatchResult_dealloc(MatchResult *self);

char *MatchResult_repr(MatchResult *self, char *head, uint8_t indent);

#ifdef __cplusplus
}
#endif
