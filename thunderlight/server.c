#include "server.h"


static int Server_init(Server *self, PyObject *args, PyObject *kwds) {
    PyArg_ParseTuple(args, "Ol", &self->app, &self->port);
    Py_INCREF(((Server *)self)->app);
    return 0;
}

static void Server_dealloc(Server *self) {
    Py_DECREF(self->app);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ServerType = {
    .tp_name = "server.Server",
    .tp_itemsize = sizeof(Server),
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)Server_init,
    .tp_dealloc = (destructor)Server_dealloc,
};

static PyModuleDef Server_module = {
    PyModuleDef_HEAD_INIT,
    "server",
    "server",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_server(void) {
    PyObject* m = NULL;
    if (PyType_Ready(&ServerType) < 0) {
        goto error;
    }
    m = PyModule_Create(&Server_module);
    Py_INCREF(&ServerType);
    PyModule_AddObject(m, "Server", (PyObject *)&ServerType);
    if (!m) {
        goto error;
    }
    goto finally;
error:
    m = NULL;
finally:
    return m;
}
