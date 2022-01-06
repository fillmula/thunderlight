#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "request.h"

typedef struct {
    PyObject_HEAD
    PyObject *app;
    PyObject *transport;
    PyObject *req;
    Request request;
} Protocol;

#ifdef __cplusplus
}
#endif
