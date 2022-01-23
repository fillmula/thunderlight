#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include <stdbool.h>
#include "request.h"


#define MATCHER_LIST_INITIAL_SIZE 32
#define MATCHER_MAX_ITEMS 16

typedef struct {
    const char *route;
    PyObject *handler;
    bool is_static;
    uint8_t len;
    uint8_t ele_lens[MATCHER_MAX_ITEMS];
    uint8_t ele_num;
    bool statics[MATCHER_MAX_ITEMS];
    char *keys[MATCHER_MAX_ITEMS];
} Matcher;

typedef struct {
    Matcher *buffer;
    Matcher inline_buffer[MATCHER_LIST_INITIAL_SIZE];
    size_t length;
    size_t capacity;
} MatcherList;

MatcherList *MatcherList_alloc(void);

void MatcherList_init(MatcherList *self);

void MatcherList_dealloc(MatcherList *self);

void MatcherList_append(MatcherList *self, const char *route, PyObject *handler);

PyObject *MatcherList_match(MatcherList *self, char *path, Request *request);

#ifdef __cplusplus
}
#endif
