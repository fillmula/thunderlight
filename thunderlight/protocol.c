#include <Python.h>
#include "protocol.h"


static PyObject *Protocol_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Protocol *self = NULL;
    self = (Protocol *)type->tp_alloc(type, 0);
    self->transport = NULL;
    Request_init(&(self->request));
    Response_init(&(self->response));
    Duostate_init(&(self->duostate));
    Context_init(&(self->context), &(self->request), &(self->response), &(self->duostate));
    self->ctx = Ctx_new(&(self->context));
    return (PyObject *)self;
}

static int Protocol_init(Protocol *self, PyObject *args, PyObject *kwds) {
    int result = 0;
    if(!PyArg_ParseTuple(args, "O", &self->app)) {
        result = -1;
    }
    Py_XINCREF(self->app);
    return result;
}

static void Protocol_dealloc(Protocol *self) {
    Py_XDECREF(self->app);
    Py_XDECREF(self->transport);
    Py_XDECREF(self->ctx);
    Request_dealloc(&(self->request));
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Protocol_connection_made(Protocol *self, PyObject *transport) {
    self->transport = transport;
    Py_XINCREF(self->transport);
    Py_RETURN_NONE;
}

static PyObject *Protocol_connection_lost(Protocol *self, PyObject *exc) {
    Py_XDECREF(self->transport);
    self->transport = NULL;
    Py_RETURN_NONE;
}

static PyObject *Protocol_data_received(Protocol *self, PyObject *data) {
    char *content;
    Py_ssize_t len;
    PyBytes_AsStringAndSize(data, &content, &len);
    RequestParsingState state = Request_receive(&(self->request), content, len);
    Py_XDECREF(data);
    if (state == RequestParsingStateDone) {
        App_process(self->app, self);
    }
    Py_RETURN_NONE;
}

void Protocol_complete(Protocol *self) {
    size_t header_len;
    char *headers = Response_get_header_bytes(&self->response, &header_len);
    size_t body_len;
    char *body = Response_get_body_bytes(&self->response, &body_len);
    PyObject *header_bytes = PyBytes_FromStringAndSize((const char *)headers, header_len);
    PyObject *body_bytes = PyBytes_FromStringAndSize((const char *)body, body_len);
    PyObject *transport_write = PyObject_GetAttrString(self->transport, "write");
    PyObject_CallOneArg(transport_write, header_bytes);
    PyObject_CallOneArg(transport_write, body_bytes);
    Py_DECREF(transport_write);
    PyObject *transport_close = PyObject_GetAttrString(self->transport, "close");
    PyObject_CallNoArgs(transport_close);
    Py_DECREF(transport_close);
}

PyObject *Protocol_call(PyObject *protocol, PyObject *args, PyObject *kwds) {
    Protocol_complete((Protocol *)protocol);
    Py_RETURN_NONE;
}

static PyMethodDef Protocol_methods[] = {
    {"connection_made", (PyCFunction)Protocol_connection_made, METH_O, ""},
    {"connection_lost", (PyCFunction)Protocol_connection_lost, METH_VARARGS, ""},
    {"data_received", (PyCFunction)Protocol_data_received, METH_O, ""},
    {NULL}
};

static PyTypeObject ProtocolType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Protocol",
    .tp_basicsize = sizeof(Protocol),
    .tp_dealloc = (destructor)Protocol_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = Protocol_methods,
    .tp_new = Protocol_new,
    .tp_init = (initproc)Protocol_init,
    .tp_call = Protocol_call
};


static PyModuleDef protocol = {
    PyModuleDef_HEAD_INIT,
    "protocol",
    "protocol",
    -1,
    NULL, NULL, NULL, NULL, NULL
};
