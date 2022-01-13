#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "response.h"


typedef struct {
    PyObject_HEAD
    Response *response;
    PyObject *code;
    PyObject *headers;
    PyObject *body;
} Res;

static PyTypeObject ResType;

PyObject *Res_new(Response *response);

#ifdef __cplusplus
}
#endif
