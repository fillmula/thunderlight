#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "request.h"

typedef struct {
    PyObject_HEAD
    Request *request;
} ReadonlyHeaderMap;

typedef struct {
    PyObject_HEAD
    Request *request;
    PyObject *args;
    PyObject *method;
    PyObject *path;
    PyObject *query;
    PyObject *version;
    PyObject *headers;
    PyObject *body;
} Req;

extern PyTypeObject ReqType;

PyObject *Req_new(Request *request);

void Req_dealloc(Req *self);

#ifdef __cplusplus
}
#endif
