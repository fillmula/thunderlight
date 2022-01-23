#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

extern PyTypeObject ApplierType;

extern PyTypeObject AppliedHandlerType;

extern PyTypeObject AppliedHandlerIteratorType;

typedef struct {
    PyObject_HEAD
    PyObject *middleware;
} Applier;

PyObject *Applier_new(PyObject *middleware);

#ifdef __cplusplus
}
#endif
