#include "server.h"
#include "protocol.h"
#include "loop.h"


int Server_init(Server *self, PyObject *args, PyObject *kwds) {
    PyArg_ParseTuple(args, "OO", &self->app, &self->port);
    Py_INCREF(self->app);
    Py_INCREF(self->app);
    Py_INCREF(self->port);
    App_prepare(self->app);
    return 0;
}

void Server_dealloc(Server *self) {
    Py_DECREF(self->app);
    Py_DECREF(self->port);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Server_call(Server *self, PyObject *args, PyObject *kwargs) {
    return (PyObject *)Protocol_native_new(self->app);
}

PyObject *Server_listen(Server *self) {
    PyObject *uvloop = PyImport_ImportModule("uvloop");
    PyObject *new_event_loop = PyObject_GetAttrString(uvloop, "new_event_loop");
    PyObject *loop = PyObject_CallNoArgs(new_event_loop);
    Loop_set(loop);
    Py_INCREF(loop);
    Py_INCREF(loop);
    PyObject *asyncio = PyImport_ImportModule("asyncio");
    Py_INCREF(asyncio);
    PyObject *set_event_loop = PyObject_GetAttrString(asyncio, "set_event_loop");
    PyObject_CallOneArg(set_event_loop, loop);
    PyObject *create_server = PyObject_GetAttrString(loop, "create_server");
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, (PyObject *)self);
    PyObject *kwargs = PyDict_New();
    PyDict_SetItemString(kwargs, "port", self->port);
    PyObject *server_coro = PyObject_Call(create_server, args, kwargs);
    Py_DECREF(args);
    Py_DECREF(kwargs);
    PyObject *run_until_complete = PyObject_GetAttrString(loop, "run_until_complete");
    PyObject *server = PyObject_CallOneArg(run_until_complete, server_coro);
    // port is used
    if (server == NULL) {
        PyErr_Clear();
        long port = PyLong_AsLong(self->port);
        printf("\033[1;34m[THUNDERLIGHT]\033[22;0m \033[1;31m[ERROR]\033[22;0m port %ld is used.\n", port);
        Py_RETURN_NONE;
    }
    // port is not used and server is created successfully
    PyObject *run_forever = PyObject_GetAttrString(loop, "run_forever");
    PyObject *add_signal_handler = PyObject_GetAttrString(loop, "add_signal_handler");
    PyObject *stop = PyObject_GetAttrString(loop, "stop");
    PyObject *sigterm = PyLong_FromLong(SIGTERM);
    PyObject *sigint = PyLong_FromLong(SIGINT);
    PyObject *sigterm_args = PyTuple_New(2);
    PyTuple_SetItem(sigterm_args, 0, sigterm);
    PyTuple_SetItem(sigterm_args, 1, stop);
    PyObject_Call(add_signal_handler, sigterm_args, NULL);
    PyObject *sigint_args = PyTuple_New(2);
    PyTuple_SetItem(sigint_args, 0, sigint);
    PyTuple_SetItem(sigint_args, 1, stop);
    PyObject_Call(add_signal_handler, sigint_args, NULL);
    Py_DECREF(sigterm_args);
    Py_DECREF(sigint_args);
    Py_DECREF(sigterm);
    Py_DECREF(sigint);
    // run forever never return
    long port = PyLong_AsLong(self->port);
    printf("\033[1;34m[THUNDERLIGHT]\033[22;0m \033[1;33m[INFO]\033[22;0m server is started, listening on port %ld.\n", port);
    PyObject *none = PyObject_CallNoArgs(run_forever);
    // perform cleanup
    // server.close()
    // loop.run_until_complete(server.wait_closed())
    // loop.close()
    printf("\n\033[1;34m[THUNDERLIGHT]\033[22;0m \033[1;33m[INFO]\033[22;0m server is stopped, see you.\n");
    Py_RETURN_NONE;
}

PyMethodDef Server_methods[] = {
    {"listen", (PyCFunction)Server_listen, METH_NOARGS, "Start the server."},
    {NULL}
};

PyTypeObject ServerType = {
    .tp_name = "thunderlight.Server",
    .tp_basicsize = sizeof(Server),
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)Server_init,
    .tp_dealloc = (destructor)Server_dealloc,
    .tp_call = (ternaryfunc)Server_call,
    .tp_methods = Server_methods
};
