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

PyObject *Res_new(Response *response);

void Res_set_length_header(Res *self);

#ifdef __cplusplus
}
#endif
