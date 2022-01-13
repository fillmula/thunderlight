#include "middleware_chain.h"


static PyTypeObject OuterNextType;
static PyTypeObject OuterNextIteratorType;
static PyTypeObject ChainedMiddlewareType;
static PyTypeObject ChainedMiddlewareIteratorType;

static OuterNext *OuterNext_new(PyObject *inner, PyObject *next) {
    OuterNext *self = (OuterNext *)OuterNextType.tp_alloc(&OuterNextType, 0);
    self->inner = inner;
    self->next = next;
    Py_INCREF(self->inner);
    Py_INCREF(self->next);
    return self;
}

static void OuterNext_dealloc(OuterNext *self) {
    Py_DECREF(self->inner);
    Py_DECREF(self->next);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static OuterNextIterator *OuterNextIterator_new(OuterNext *outer_next, PyObject *ctx) {
    OuterNextIterator *self = (OuterNextIterator *)OuterNextIteratorType.tp_alloc(&OuterNextIteratorType, 0);
    self->outer_next = outer_next;
    self->ctx = ctx;
    Py_INCREF(self->outer_next);
    Py_INCREF(self->ctx);
    return self;
}

static void OuterNextIterator_dealloc(OuterNextIterator *self) {
    Py_DECREF(self->outer_next);
    Py_DECREF(self->ctx);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static OuterNextIterator *OuterNext_call(OuterNext *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx;
    PyArg_ParseTuple(args, "O", &ctx);
    return OuterNextIterator_new(self, ctx);
}

static PyTypeObject OuterNextType = {
    .tp_name = "_OuterNext",
    .tp_doc = "OuterNext",
    .tp_basicsize = sizeof(OuterNext),
    .tp_dealloc = (destructor)OuterNext_dealloc,
    .tp_call = (ternaryfunc)OuterNext_call
};

static PyObject *OuterNextIterator_anext(OuterNextIterator *self) {
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, self->ctx);
    PyObject *result = PyObject_Call(self->outer_next->inner, args, NULL);
    Py_DECREF(args);
    return result;
}

static PyObject *OuterNextIterator_await(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject *OuterNextIterator_aiter(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyAsyncMethods OuterNextIterator_async_methods = {
    .am_anext = (unaryfunc)OuterNextIterator_anext,
    .am_await = OuterNextIterator_await,
    .am_aiter = OuterNextIterator_aiter
};

static PyTypeObject OuterNextIteratorType = {
    .tp_name = "_OuterNextIterator",
    .tp_doc = "OuterNextIterator",
    .tp_basicsize = sizeof(OuterNextIterator),
    .tp_dealloc = (destructor)OuterNextIterator_dealloc,
    .tp_as_async = &OuterNextIterator_async_methods
};

static ChainedMiddleware *ChainedMiddleware_new(PyObject *outer, PyObject *inner) {
    ChainedMiddleware *self = (ChainedMiddleware *)ChainedMiddlewareType.tp_alloc(&ChainedMiddlewareType, 0);
    self->outer = outer;
    self->inner = inner;
    Py_INCREF(outer);
    Py_INCREF(inner);
    return self;
}

static void ChainedMiddleware_dealloc(ChainedMiddleware *self) {
    Py_DECREF(self->outer);
    Py_DECREF(self->inner);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static ChainedMiddlewareIterator *ChainedMiddlewareIterator_new(ChainedMiddleware *chained_middleware, PyObject *ctx, PyObject *next) {
    ChainedMiddlewareIterator *self = (ChainedMiddlewareIterator *)ChainedMiddlewareIteratorType.tp_alloc(&ChainedMiddlewareIteratorType, 0);
    self->chained_middleware = chained_middleware;
    self->ctx = ctx;
    self->next = next;
    Py_INCREF(chained_middleware);
    Py_INCREF(ctx);
    Py_INCREF(next);
    return self;
}

static void ChainedMiddlewareIterator_dealloc(ChainedMiddlewareIterator *self) {
    Py_DECREF(self->chained_middleware);
    Py_DECREF(self->ctx);
    Py_DECREF(self->next);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static ChainedMiddlewareIterator *ChainedMiddleware_call(ChainedMiddleware *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx;
    PyObject *next;
    PyArg_ParseTuple(args, "OO", &ctx, &next);
    return ChainedMiddlewareIterator_new(self, ctx, next);
}

static PyTypeObject ChainedMiddlewareType = {
    .tp_name = "_ChainedMiddleware",
    .tp_doc = "ChainedMiddleware",
    .tp_basicsize = sizeof(ChainedMiddleware),
    .tp_call = (ternaryfunc)ChainedMiddleware_call,
    .tp_dealloc = (destructor)ChainedMiddleware_dealloc
};

static PyObject *ChainedMiddlewareIterator_aiter(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject *ChainedMiddlewareIterator_await(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject *ChainedMiddlewareIterator_anext(ChainedMiddlewareIterator *self) {
    PyObject *outer_next = (PyObject *)OuterNext_new(self->chained_middleware->inner, self->next);
    PyObject *args = PyTuple_New(2);
    PyTuple_SetItem(args, 0, self->ctx);
    PyTuple_SetItem(args, 1, outer_next);
    PyObject *result = PyObject_Call(self->chained_middleware->outer, args, NULL);
    Py_DECREF(args);
    return result;
}

static PyAsyncMethods ChainedMiddlewareIterator_async_methods = {
    .am_aiter = ChainedMiddlewareIterator_aiter,
    .am_anext = (unaryfunc)ChainedMiddlewareIterator_anext,
    .am_await = ChainedMiddlewareIterator_await
};

static PyTypeObject ChainedMiddlewareIteratorType = {
    .tp_name = "_ChainedMiddlewareIterator",
    .tp_doc = "ChainedMiddlewareIterator",
    .tp_basicsize = sizeof(ChainedMiddlewareIterator),
    .tp_as_async = &ChainedMiddlewareIterator_async_methods,
    .tp_dealloc = (destructor)ChainedMiddlewareIterator_dealloc
};

PyObject *ChainedMiddleware_build(PyObject *list) {
    Py_ssize_t size = PyList_Size(list);
    switch (size) {
        case 0:
            return NULL;
        case 1:
            return PyList_GetItem(list, 0);
        default: {
            PyObject *middleware = PyList_GetItem(list, size - 1);
            for (size_t i = size - 2; i >= 0; i--) {
                middleware = (PyObject *)ChainedMiddleware_new(PyList_GetItem(list, i), middleware);
            }
            return middleware;
        }
    }
}
