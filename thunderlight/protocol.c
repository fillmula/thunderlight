#include <Python.h>

#include "protocol.h"


static PyObject *Protocol_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Protocol *self = NULL;
    self = (Protocol *)type->tp_alloc(type, 0);
    self->app = NULL;
    self->transport = NULL;
    self->req = NULL;
    Buffer_init(&(self->buffer));
    Request_init(self->request, &(self->buffer));
    return (PyObject *)self;
}

static void Protocol_dealloc(Protocol *self) {
    Py_XDECREF(self->app);
    Py_XDECREF(self->transport);
    Py_XDECREF(self->req);
    if(self->buffer.content != self->buffer.inline_content) {
        free(self->buffer.content);
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int Protocol_init(Protocol *self, PyObject *args, PyObject *kwds) {
    int result = 0;
    if(!PyArg_ParseTuple(args, "O", &self->app)) {
        result = -1;
    }
    Py_INCREF(self->app);
    return result;
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
    Buffer_append(&(self->buffer), content, len);
    Py_XDECREF(data);
    Request_buffer_sync(&(self->request));
    Request_parse(&(self->request));
    Py_RETURN_NONE;
}

// static PyMethodDef Protocol_methods[] = {
//   {"connection_made", (PyCFunction)Protocol_connection_made, METH_O, ""},
//   {"connection_lost", (PyCFunction)Protocol_connection_lost, METH_VARARGS, ""},
//   {"data_received", (PyCFunction)Protocol_data_received, METH_O, ""},
//   {"pipeline_cancel", (PyCFunction)Protocol_pipeline_cancel, METH_NOARGS, ""},
// #ifdef PARSER_STANDALONE
//   {"on_headers", (PyCFunction)Protocol_on_headers, METH_VARARGS, ""},
//   {"on_body", (PyCFunction)Protocol_on_body, METH_VARARGS, ""},
//   {"on_error", (PyCFunction)Protocol_on_error, METH_VARARGS, ""},
// #endif
//   {NULL}
// };


static PyTypeObject ProtocolType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "protocol.Protocol",       /* tp_name */
  sizeof(Protocol),          /* tp_basicsize */
  0,                         /* tp_itemsize */
  (destructor)Protocol_dealloc, /* tp_dealloc */
  0,                         /* tp_print */
  0,                         /* tp_getattr */
  0,                         /* tp_setattr */
  0,                         /* tp_reserved */
  0,                         /* tp_repr */
  0,                         /* tp_as_number */
  0,                         /* tp_as_sequence */
  0,                         /* tp_as_mapping */
  0,                         /* tp_hash  */
  0,                         /* tp_call */
  0,                         /* tp_str */
  0,                         /* tp_getattro */
  0,                         /* tp_setattro */
  0,                         /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,        /* tp_flags */
  "Protocol",                /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  Protocol_methods,          /* tp_methods */
  0,                         /* tp_members */
  Protocol_getset,           /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)Protocol_init,   /* tp_init */
  0,                         /* tp_alloc */
  Protocol_new,              /* tp_new */
};


static PyModuleDef cprotocol = {
  PyModuleDef_HEAD_INIT,
  "protocol",
  "protocol",
  -1,
  NULL, NULL, NULL, NULL, NULL
};
