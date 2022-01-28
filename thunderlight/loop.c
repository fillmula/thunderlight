#include "loop.h"

PyObject *uvloop_loop = NULL;
PyObject *asyncio = NULL;
PyObject *ensure_future = NULL;

void Loop_set(PyObject *loop) {
    Py_XDECREF(uvloop_loop);
    Py_INCREF(loop);
    uvloop_loop = loop;
    asyncio = PyImport_ImportModule("asyncio");
    ensure_future = PyObject_GetAttrString(asyncio, "ensure_future");
}

PyObject *Loop_start_awaitable(PyObject *awaitable) {
    Py_INCREF(awaitable);
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, awaitable);
    PyObject *kwargs = PyDict_New();
    PyDict_SetItemString(kwargs, "loop", uvloop_loop);
    PyObject *future = PyObject_Call(ensure_future, args, kwargs);
    Py_DECREF(args);
    Py_DECREF(kwargs);
    Py_INCREF(future);
    return future;
}
