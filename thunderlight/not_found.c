#include "not_found.h"


static PyTypeObject NotFoundIteratorType;

static NotFoundIterator *NotFoundIterator_new(Ctx *ctx) {
    NotFoundIterator *self = (NotFoundIterator *)NotFoundIteratorType.tp_alloc(&NotFoundIteratorType, 0);
    Py_INCREF(ctx);
    self->ctx = ctx;
    return self;
}

static void NotFoundIterator_dealloc(NotFoundIterator *self) {
    Py_DECREF(self->ctx);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *NotFoundIterator_aiter(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject *NotFoundIterator_await(PyObject *self) {
    Py_INCREF(self);
    return self;
}

static PyObject *NotFoundIterator_anext(NotFoundIterator *self) {
    self->ctx->context->response->code = 404;
    self->ctx->context->response->body = "{\"error\": {\"type\": \"NotFound\", \"message\": \"This location is not found.\"}}";
    self->ctx->context->response->body_len = 73;
    PyErr_SetString(PyExc_StopAsyncIteration, "");
    return NULL;
}

static PyAsyncMethods NotFoundIterator_async_methods = {
    .am_aiter = NotFoundIterator_aiter,
    .am_await = NotFoundIterator_await,
    .am_anext = (unaryfunc)NotFoundIterator_anext
};

static PyTypeObject NotFoundIteratorType = {
    .tp_name = "NotFoundIterator",
    .tp_doc = "NotFoundIterator",
    .tp_alloc = PyType_GenericAlloc,
    .tp_dealloc = (destructor)NotFoundIterator_dealloc,
    .tp_as_async = &NotFoundIterator_async_methods
};

static int NotFound_init(NotFound *self, PyObject *args, PyObject *kwds) {
    return 0;
}

static void NotFound_dealloc(NotFound *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static NotFoundIterator *NotFound_call(NotFound *self, PyObject *args, PyObject *kwds) {
    PyObject *ctx = NULL;
    PyArg_ParseTuple(args, "O", &ctx);
    return NotFoundIterator_new((Ctx *)ctx);
}

static PyTypeObject NotFoundType = {
    .tp_name = "NotFound",
    .tp_doc = "NotFound",
    .tp_new = PyType_GenericNew,
    .tp_alloc = PyType_GenericAlloc,
    .tp_init = (initproc)NotFound_init,
    .tp_dealloc = (destructor)NotFound_dealloc,
    .tp_call = (ternaryfunc)NotFound_call,
    .tp_itemsize = sizeof(NotFound)
};

static PyModuleDef NotFoundModule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "handlers.not_found",
    .m_doc = "handlers.not_found",
    .m_size = -1
};

PyMODINIT_FUNC PyInit_not_found(void) {
    PyObject *m = NULL;
    if (PyType_Ready(&NotFoundIteratorType) < 0) {
        goto error;
    }
    if (PyType_Ready(&NotFoundType) < 0) {
        goto error;
    }
    m = PyModule_Create(&NotFoundModule);
    Py_INCREF(&NotFoundType);
    PyModule_AddObject(m, "NotFound", (PyObject *)&NotFoundType);
    Py_INCREF(&NotFoundIteratorType);
    PyModule_AddObject(m, "NotFoundIterator", (PyObject *)&NotFoundIteratorType);
    not_found = PyType_GenericNew(&NotFoundType, NULL, NULL);
    Py_INCREF(not_found);
    PyModule_AddObject(m, "not_found", not_found);
error:
    m = NULL;
finally:
    return m;
}
