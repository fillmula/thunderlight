#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "ctx.h"


typedef struct {
    PyObject_HEAD
    Ctx *ctx;
} NotFoundIterator;

typedef struct {
    PyObject_HEAD
} NotFound;

extern PyTypeObject NotFoundType;

extern PyTypeObject NotFoundIteratorType;

extern PyObject *not_found;

#ifdef __cplusplus
}
#endif
