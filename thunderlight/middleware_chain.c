#include "middleware_chain.h"
#include "loop.h"


PyTypeObject OuterNextType;
PyTypeObject OuterNextIteratorType;
PyTypeObject ChainedMiddlewareType;
PyTypeObject ChainedMiddlewareIteratorType;

OuterNext *OuterNext_new(PyObject *inner, PyObject *next) {
    OuterNext *self = (OuterNext *)OuterNextType.tp_alloc(&OuterNextType, 0);
    self->inner = inner;
    self->next = next;
    Py_INCREF(self->inner);
    Py_INCREF(self->next);
    Py_INCREF(self->inner);
    Py_INCREF(self->next);
    return self;
}

void OuterNext_dealloc(OuterNext *self) {
    Py_DECREF(self->inner);
    Py_DECREF(self->next);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

OuterNextIterator *OuterNextIterator_new(OuterNext *outer_next, PyObject *ctx) {
    OuterNextIterator *self = (OuterNextIterator *)OuterNextIteratorType.tp_alloc(&OuterNextIteratorType, 0);
    self->outer_next = outer_next;
    self->ctx = ctx;
    Py_INCREF(self->outer_next);
    Py_INCREF(self->ctx);
    Py_INCREF(self->outer_next);
    Py_INCREF(self->ctx);
    self->state = 0;
    self->future = NULL;
    return self;
}

void OuterNextIterator_dealloc(OuterNextIterator *self) {
    Py_DECREF(self->outer_next);
    Py_DECREF(self->ctx);
    Py_XDECREF(self->future);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

OuterNextIterator *OuterNext_call(OuterNext *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx;
    PyArg_ParseTuple(args, "O", &ctx);
    return OuterNextIterator_new(self, ctx);
}

PyTypeObject OuterNextType = {
    .tp_name = "thunderlight._OuterNext",
    .tp_doc = "OuterNext",
    .tp_basicsize = sizeof(OuterNext),
    .tp_dealloc = (destructor)OuterNext_dealloc,
    .tp_call = (ternaryfunc)OuterNext_call
};


PyObject *OuterNextIterator_await(OuterNextIterator *self) {
    Py_INCREF(self);
    return self;
}

PyObject *OuterNextIterator_iter(OuterNextIterator *self) {
    Py_INCREF(self);
    return self;
}

PyObject *OuterNextIterator_iternext(OuterNextIterator *self) {
    if (PyErr_Occurred() != NULL) {
        PyErr_SetObject(PyExc_Exception, PyErr_Occurred());
        return NULL;
    }
    if (self->future == NULL) {
        PyObject *args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, self->ctx);
        PyTuple_SetItem(args, 1, self->outer_next->next);
        PyObject *result = PyObject_Call(self->outer_next->inner, args, NULL);
        Py_DECREF(args);
        PyObject *future = Loop_start_awaitable(result);
        Py_INCREF(future);
        Py_INCREF(future);
        Py_INCREF(future);
        self->future = future;
    }
    PyObject *done = PyObject_GetAttrString(self->future, "done");
    PyObject *is_done = PyObject_CallNoArgs(done);
    if (PyObject_IsTrue(is_done)) {
        PyObject *exception = PyObject_GetAttrString(self->future, "exception");
        PyObject *exc = PyObject_CallNoArgs(exception);
        if (Py_IsNone(exc)) {
            PyErr_SetNone(PyExc_StopIteration);
            return NULL;
        } else {
            PyErr_SetObject(PyExc_Exception, exc);
            return NULL;
        }
    } else {
        Py_RETURN_NONE;
    }
}

PyAsyncMethods OuterNextIterator_async_methods = {
    .am_anext = NULL,
    .am_await = OuterNextIterator_await,
    .am_aiter = NULL,
    .am_send = NULL
};

PyTypeObject OuterNextIteratorType = {
    .tp_name = "thunderlight._OuterNextIterator",
    .tp_doc = "OuterNextIterator",
    .tp_basicsize = sizeof(OuterNextIterator),
    .tp_dealloc = (destructor)OuterNextIterator_dealloc,
    .tp_as_async = &OuterNextIterator_async_methods,
    .tp_iter = (getiterfunc)OuterNextIterator_iter,
    .tp_iternext = (iternextfunc)OuterNextIterator_iternext
};

ChainedMiddleware *ChainedMiddleware_new(PyObject *outer, PyObject *inner) {
    ChainedMiddleware *self = (ChainedMiddleware *)ChainedMiddlewareType.tp_alloc(&ChainedMiddlewareType, 0);
    self->outer = outer;
    self->inner = inner;
    Py_INCREF(outer);
    Py_INCREF(inner);
    Py_INCREF(outer);
    Py_INCREF(inner);
    return self;
}

void ChainedMiddleware_dealloc(ChainedMiddleware *self) {
    Py_DECREF(self->outer);
    Py_DECREF(self->inner);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

ChainedMiddlewareIterator *ChainedMiddlewareIterator_new(ChainedMiddleware *chained_middleware, PyObject *ctx, PyObject *next) {
    ChainedMiddlewareIterator *self = (ChainedMiddlewareIterator *)ChainedMiddlewareIteratorType.tp_alloc(&ChainedMiddlewareIteratorType, 0);
    self->chained_middleware = chained_middleware;
    self->ctx = ctx;
    self->next = next;
    self->state = 0;
    Py_INCREF(chained_middleware);
    Py_INCREF(ctx);
    Py_INCREF(next);
    Py_INCREF(chained_middleware);
    Py_INCREF(ctx);
    Py_INCREF(next);
    self->future = NULL;
    return self;
}

void ChainedMiddlewareIterator_dealloc(ChainedMiddlewareIterator *self) {
    Py_DECREF(self->chained_middleware);
    Py_DECREF(self->ctx);
    Py_DECREF(self->next);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

ChainedMiddlewareIterator *ChainedMiddleware_call(ChainedMiddleware *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx;
    PyObject *next;
    PyArg_ParseTuple(args, "OO", &ctx, &next);
    return ChainedMiddlewareIterator_new(self, ctx, next);
}

PyTypeObject ChainedMiddlewareType = {
    .tp_name = "thunderlight._ChainedMiddleware",
    .tp_doc = "ChainedMiddleware",
    .tp_basicsize = sizeof(ChainedMiddleware),
    .tp_call = (ternaryfunc)ChainedMiddleware_call,
    .tp_dealloc = (destructor)ChainedMiddleware_dealloc
};


PyObject *ChainedMiddlewareIterator_await(ChainedMiddlewareIterator *self) {
    Py_INCREF(self);
    return self;
}

PyObject *ChainedMiddlewareIterator_iter(ChainedMiddlewareIterator *self) {
    Py_INCREF(self);
    return self;
}

PyObject *ChainedMiddlewareIterator_iternext(ChainedMiddlewareIterator *self) {
    if (PyErr_Occurred() != NULL) {
        PyErr_SetObject(PyExc_Exception, PyErr_Occurred());
        return NULL;
    }
    if (self->future == NULL) {
        PyObject *outer_next = (PyObject *)OuterNext_new(self->chained_middleware->inner, self->next);
        PyObject *args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, self->ctx);
        PyTuple_SetItem(args, 1, outer_next);
        PyObject *result = PyObject_Call(self->chained_middleware->outer, args, NULL);
        Py_DECREF(args);
        PyObject *future = Loop_start_awaitable(result);
        Py_INCREF(future);
        self->future = future;
    }
    PyObject *done = PyObject_GetAttrString(self->future, "done");
    PyObject *is_done = PyObject_CallNoArgs(done);
    if (PyObject_IsTrue(is_done)) {
        PyObject *exception = PyObject_GetAttrString(self->future, "exception");
        PyObject *exc = PyObject_CallNoArgs(exception);
        if (Py_IsNone(exc)) {
            PyErr_SetNone(PyExc_StopIteration);
            return NULL;
        } else {
            PyErr_SetObject(PyExc_Exception, exc);
            return NULL;
        }
    } else {
        Py_RETURN_NONE;
    }
}

PyAsyncMethods ChainedMiddlewareIterator_async_methods = {
    .am_aiter = NULL,
    .am_anext = NULL,
    .am_await = ChainedMiddlewareIterator_await,
    .am_send = NULL
};

PyTypeObject ChainedMiddlewareIteratorType = {
    .tp_name = "thunderlight._ChainedMiddlewareIterator",
    .tp_doc = "ChainedMiddlewareIterator",
    .tp_basicsize = sizeof(ChainedMiddlewareIterator),
    .tp_as_async = &ChainedMiddlewareIterator_async_methods,
    .tp_dealloc = (destructor)ChainedMiddlewareIterator_dealloc,
    .tp_iter = (getiterfunc)ChainedMiddlewareIterator_iter,
    .tp_iternext = (iternextfunc)ChainedMiddlewareIterator_iternext
};

PyObject *ChainedMiddleware_build(PyObject *list) {
    Py_ssize_t len = PyObject_Length(list);
    switch (len) {
        case 0:
            return NULL;
        case 1:
            return PyList_GetItem(list, 0);
        default: {
            PyObject *middleware = PyList_GetItem(list, len - 1);
            for (Py_ssize_t i = len - 2; i >= 0; i--) {
                middleware = (PyObject *)ChainedMiddleware_new(PyList_GetItem(list, i), middleware);
            }
            return middleware;
        }
    }
}
