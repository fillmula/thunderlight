#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

void Loop_set(PyObject *loop);

PyObject *Loop_start_awaitable(PyObject *awaitable);

#ifdef __cplusplus
}
#endif
