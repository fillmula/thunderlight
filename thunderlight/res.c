#include "res.h"
#include "res_headers.h"


static PyTypeObject ReqType;

PyObject *Res_new(Response *response) {
    Res *self = NULL;
    self = (Res *)ReqType.tp_alloc(&ReqType, 0);
    self->response = response;
    self->code = NULL;
    self->headers = NULL;
    self->body = NULL;
    return (PyObject *)self;
}

static void Res_dealloc(Res *self) {
    Py_XDECREF(self->code);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->body);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

void Res_set_length_header(Res *self) {

}

static PyObject *Res_get_code(Res *self, void *closure) {
    if (!self->code) {
        self->code = PyLong_FromUnsignedLong((unsigned long)self->response->code);
    }
    Py_INCREF(self->code);
    return self->code;
}

static int Res_set_code(Res *self, PyObject *value, void *closure) {
    Py_INCREF(value);
    self->code = value;
    self->response->code = PyLong_AsUnsignedLong(value);
    return 0;
}

static PyObject *Res_get_headers(Res *self, void *closure) {
    if (!self->headers) {
        self->headers = (PyObject *)ResHeaders_new(&self->response->headers);
    }
    Py_INCREF(self->headers);
    return self->headers;
}

static PyObject *Res_get_body(Res *self, void *closure) {
    if (self->response->body_len == 0) {
        Py_RETURN_NONE;
    }
    if (!self->body) {
        self->body = PyBytes_FromStringAndSize(self->response->body, self->response->body_len);
    }
    Py_INCREF(self->body);
    return self->body;
}

static int Res_set_body(Res *self, PyObject *value, void *closure) {
    return 0;
}

static PyGetSetDef Res_getset[] = {
    {"code", (getter)Res_get_code, (setter)Res_set_code, NULL, NULL},
    {"headers", (getter)Res_get_headers, NULL, NULL, NULL},
    {"body", (getter)Res_get_body, (setter)Res_set_body, NULL, NULL},
    {NULL}
};

static PyTypeObject ResType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "res.Res",
    .tp_basicsize = sizeof(Res),
    .tp_dealloc = (destructor)Res_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = Res_getset,
};

static PyModuleDef Res_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "res",
    .m_doc = "res",
    .m_size = -1
};

PyMODINIT_FUNC
PyInit_res(void) {
    PyObject* m = NULL;
    if (PyType_Ready(&ResType) < 0) {
        goto error;
    }
    m = PyModule_Create(&Res_module);
    Py_INCREF(&ResType);
    PyModule_AddObject(m, "Res", (PyObject *)&ResType);
    if (!m) {
        goto error;
    }
    goto finally;
error:
    m = NULL;
finally:
    return m;
}
