#include "applier.h"
#include "loop.h"


typedef struct {
    PyObject_HEAD
    PyObject *middleware;
    PyObject *handler;
} AppliedHandler;

typedef struct {
    PyObject_HEAD
    AppliedHandler *applied_handler;
    PyObject *ctx;
    PyObject *future;
} AppliedHandlerIterator;

PyTypeObject ApplierType;

PyTypeObject AppliedHandlerType;

PyTypeObject AppliedHandlerIteratorType;

PyObject *AppliedHandlerIterator_new(PyObject *applied_handler, PyObject *ctx) {
    AppliedHandlerIterator *self = (AppliedHandlerIterator *)AppliedHandlerIteratorType.tp_alloc(&AppliedHandlerIteratorType, 0);
    Py_INCREF(applied_handler);
    Py_INCREF(ctx);
    self->applied_handler = applied_handler;
    self->ctx = ctx;
    Py_INCREF(self);
    return (PyObject *)self;
}

void AppliedHandlerIterator_dealloc(AppliedHandlerIterator *self) {
    Py_DECREF(self->applied_handler);
    Py_DECREF(self->ctx);
    Py_DECREF(self->future);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *AppliedHandlerIterator_iter(PyObject *self) {
    Py_INCREF(self);
    return self;
}

PyObject *AppliedHandlerIterator_await(PyObject *self) {
    Py_INCREF(self);
    return self;
}

PyObject *AppliedHandlerIterator_iternext(AppliedHandlerIterator *self) {
    if (PyErr_Occurred() != NULL) {
        PyErr_SetObject(PyExc_Exception, PyErr_Occurred());
        return NULL;
    }
    if (!self->future) {
        PyObject *args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, self->ctx);
        PyTuple_SetItem(args, 1, self->applied_handler->handler);
        PyObject *result = PyObject_Call(self->applied_handler->middleware, args, NULL);
        Py_DECREF(args);
        PyObject *future = Loop_start_awaitable(result);
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

PyAsyncMethods AppliedHandlerIterator_async_methods = {
    .am_aiter = NULL,
    .am_anext = NULL,
    .am_await = AppliedHandlerIterator_await,
    .am_send = NULL
};

PyObject *AppliedHandler_new(PyObject *middleware, PyObject *handler) {
    AppliedHandler *self = (AppliedHandler *)AppliedHandlerType.tp_alloc(&AppliedHandlerType, 0);
    Py_INCREF(middleware);
    Py_INCREF(handler);
    self->middleware = middleware;
    self->handler = handler;
    return (PyObject *)self;
}

PyObject *AppliedHandler_call(AppliedHandler *self, PyObject *args, PyObject *kwargs) {
    Py_INCREF(self->handler);
    Py_INCREF(self->middleware);
    PyObject *ctx;
    PyArg_ParseTuple(args, "O", &ctx);
    Py_INCREF(ctx);
    return AppliedHandlerIterator_new(self, ctx);
}

void AppliedHandler_dealloc(AppliedHandler *self) {
    Py_DECREF(self->middleware);
    Py_DECREF(self->handler);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Applier_new(PyObject *middleware) {
    Applier *self = (Applier *)ApplierType.tp_alloc(&ApplierType, 0);
    Py_INCREF(middleware);
    self->middleware = middleware;
    return self;
}

PyObject *Applier_call(Applier *self, PyObject *args, PyObject *kwargs) {
    PyObject *handler;
    PyArg_ParseTuple(args, "O", &handler);
    return AppliedHandler_new(self->middleware, handler);
}

void Applier_dealloc(Applier *self) {
    Py_DECREF(self->middleware);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject AppliedHandlerType = {
    .tp_alloc = PyType_GenericAlloc,
    .tp_name = "thunderlight._AppliedHandler",
    .tp_call = (ternaryfunc)AppliedHandler_call,
    .tp_dealloc = (destructor)AppliedHandler_dealloc,
    .tp_doc = "",
    .tp_basicsize = sizeof(AppliedHandler)
};

PyTypeObject ApplierType = {
    .tp_alloc = PyType_GenericAlloc,
    .tp_name = "thunderlight._HandlerApplier",
    .tp_call = (ternaryfunc)Applier_call,
    .tp_dealloc = (destructor)Applier_dealloc,
    .tp_doc = "",
    .tp_basicsize = sizeof(Applier)
};

PyTypeObject AppliedHandlerIteratorType = {
    .tp_alloc = PyType_GenericAlloc,
    .tp_name = "thunderlight._AppliedHandlerIterator",
    .tp_dealloc = (destructor)AppliedHandlerIterator_dealloc,
    .tp_basicsize = sizeof(AppliedHandlerIterator),
    .tp_iter = (getiterfunc)AppliedHandlerIterator_iter,
    .tp_iternext = (iternextfunc)AppliedHandlerIterator_iternext,
    .tp_as_async = &AppliedHandlerIterator_async_methods
};
