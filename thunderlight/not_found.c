#include "not_found.h"


PyTypeObject NotFoundIteratorType;

NotFoundIterator *NotFoundIterator_new(Ctx *ctx) {
    NotFoundIterator *self = (NotFoundIterator *)NotFoundIteratorType.tp_alloc(&NotFoundIteratorType, 0);
    Py_INCREF(ctx);
    self->ctx = ctx;
    return self;
}

void NotFoundIterator_dealloc(NotFoundIterator *self) {
    Py_DECREF(self->ctx);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *NotFoundIterator_await(PyObject *self) {
    Py_INCREF(self);
    return self;
}

PyObject *NotFoundIterator_iter(NotFoundIterator *self) {
    Py_INCREF(self);
    return self;
}

PyObject *NotFoundIterator_iternext(NotFoundIterator *self) {
    if (PyErr_Occurred() != NULL) {
        PyErr_SetObject(PyExc_Exception, PyErr_Occurred());
        return NULL;
    }
    self->ctx->context->response->code = 404;
    HeaderMap_set(&self->ctx->context->response->headers, "Content-Type", 12, "application/json", 16);
    self->ctx->context->response->body = "{\"error\": {\"type\": \"NotFound\", \"message\": \"This location is not found.\"}}";
    self->ctx->context->response->body_len = 73;
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
}

PyAsyncMethods NotFoundIterator_async_methods = {
    .am_aiter = NULL,
    .am_await = NotFoundIterator_await,
    .am_anext = NULL
};

PyTypeObject NotFoundIteratorType = {
    .tp_name = "thunderlight._NotFoundIterator",
    .tp_doc = "NotFoundIterator",
    .tp_alloc = PyType_GenericAlloc,
    .tp_dealloc = (destructor)NotFoundIterator_dealloc,
    .tp_as_async = &NotFoundIterator_async_methods,
    .tp_iter = (getiterfunc)NotFoundIterator_iter,
    .tp_iternext = (iternextfunc)NotFoundIterator_iternext
};

int NotFound_init(NotFound *self, PyObject *args, PyObject *kwds) {
    return 0;
}

void NotFound_dealloc(NotFound *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

NotFoundIterator *NotFound_call(NotFound *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx = NULL;
    PyArg_ParseTuple(args, "O", &ctx);
    return NotFoundIterator_new((Ctx *)ctx);
}

PyTypeObject NotFoundType = {
    .tp_name = "thunderlight._NotFound",
    .tp_doc = "NotFound",
    .tp_new = PyType_GenericNew,
    .tp_alloc = PyType_GenericAlloc,
    .tp_init = (initproc)NotFound_init,
    .tp_dealloc = (destructor)NotFound_dealloc,
    .tp_call = (ternaryfunc)NotFound_call,
    .tp_basicsize = sizeof(NotFound)
};

PyObject *not_found = NULL;
