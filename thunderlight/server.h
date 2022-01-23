#pragma once

#include <Python.h>
#include "app.h"


typedef struct {
    PyObject_HEAD
    App *app;
    PyObject *port;
} Server;

extern PyTypeObject ServerType;