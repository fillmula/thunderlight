#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "response_headers.h"


typedef struct {
    PyObject_HEAD
    HeaderMap *header_map;
} ResHeaders;

ResHeaders *ResHeaders_new(HeaderMap *header_map);

void ResHeaders_dealloc(ResHeaders *self);

#ifdef __cplusplus
}
#endif
