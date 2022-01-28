#include <Python.h>
#include "protocol.h"

PyTypeObject ProtocolType;

Protocol *Protocol_native_new(App *app) {
    Protocol *self = (Protocol *)ProtocolType.tp_alloc(&ProtocolType, 0);
    self->transport = NULL;
    Request_init(&self->request);
    Response_init(&self->response);
    Duostate_init(&self->duostate);
    Context_init(&self->context, &self->request, &self->response, &self->duostate);
    self->ctx = Ctx_new(&self->context);
    self->app = app;
    Py_INCREF(app);
    return self;
}

int Protocol_init(Protocol *self, PyObject *args, PyObject *kwds) {
    if(!PyArg_ParseTuple(args, "O", &self->app)) {
        return -1;
    }
    Py_XINCREF(self->app);
    self->transport = NULL;
    Request_init(&(self->request));
    Response_init(&(self->response));
    Duostate_init(&(self->duostate));
    Context_init(&(self->context), &(self->request), &(self->response), &(self->duostate));
    self->ctx = Ctx_new(&(self->context));
    return 0;
}

PyObject *Protocol_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Protocol *self = (Protocol *)type->tp_alloc(type, 0);
    Protocol_init(self, args, kwds);
    return (PyObject *)self;
}

void Protocol_dealloc(Protocol *self) {
    Py_XDECREF(self->app);
    Py_XDECREF(self->transport);
    Py_XDECREF(self->ctx);
    Request_dealloc(&(self->request));
    Duostate_deinit(&self->duostate);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Protocol_connection_made(Protocol *self, PyObject *transport) {
    self->transport = transport;
    Py_XINCREF(self->transport);
    Py_RETURN_NONE;
}

PyObject *Protocol_connection_lost(Protocol *self, PyObject *exc) {
    Py_XDECREF(self->transport);
    self->transport = NULL;
    Py_RETURN_NONE;
}

PyObject *Protocol_data_received(Protocol *self, PyObject *data) {
    char *content;
    Py_ssize_t len;
    PyBytes_AsStringAndSize(data, &content, &len);
    RequestParsingState state = Request_receive(&(self->request), content, len);
    Py_XDECREF(data);
    if (state == RequestParsingStateDone) {
        // Py_INCREF(self->app);
        App_process(self->app, (PyObject *)self);
    }
    Py_RETURN_NONE;
}

void Protocol_complete(Protocol *self) {
    Response_set_version_info(&self->response, self->request.version, self->request.version_len);
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

PyMethodDef Protocol_methods[] = {
    {"connection_made", (PyCFunction)Protocol_connection_made, METH_O, ""},
    {"connection_lost", (PyCFunction)Protocol_connection_lost, METH_VARARGS, ""},
    {"data_received", (PyCFunction)Protocol_data_received, METH_O, ""},
    {NULL, NULL, 0, NULL}
};

PyTypeObject ProtocolType = {
    .tp_name = "thunderlight.Protocol",
    .tp_basicsize = sizeof(Protocol),
    .tp_dealloc = (destructor)Protocol_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = Protocol_methods,
    .tp_new = Protocol_new,
    .tp_init = (initproc)Protocol_init,
    .tp_alloc = PyType_GenericAlloc,
    .tp_call = Protocol_call,
};
