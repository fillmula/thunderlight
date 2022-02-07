#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>


void JSON_setup();

char *JSON_encode(PyObject *obj, size_t *len);

PyObject *JSON_decode(PyObject *buffer);

#ifdef __cplusplus
}
#endif
