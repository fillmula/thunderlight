#pragma once

#include <Python.h>
#include "buffer.h"
#include "request.h"


typedef struct {
    PyObject_HEAD
    PyObject *app;
    PyObject *transport;
    PyObject *req;
    Request request;
} Protocol;
