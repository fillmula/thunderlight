#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

#define INLINE_MATCHER_LIST_SIZE 32

typedef struct {
    char *route;
    PyObject *awaitable;
    bool is_static;
    uint8_t ele_num;
} Matcher;

typedef struct {
    Matcher *buffer;
    Matcher inline_buffer[INLINE_MATCHER_LIST_SIZE];
} MatcherList;

void Matcher_init(char *route, PyObject *awaitable);

void MatcherList_init();

void MatcherList_append(Matcher matcher);

void MatcherList_dealloc();

#ifdef __cplusplus
}
#endif
