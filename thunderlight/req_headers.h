#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "request.h"


typedef struct {
    PyObject_HEAD
    Request *request;
} ReqHeaders;

ReqHeaders *ReqHeaders_new(Request *request);

void ReqHeaders_dealloc(ReqHeaders *self);

#ifdef __cplusplus
}
#endif
