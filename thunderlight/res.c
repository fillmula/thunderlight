#include "res.h"
#include "res_headers.h"
#include "json.h"


PyObject *Res_new(Response *response) {
    Res *self = NULL;
    self = (Res *)ResType.tp_alloc(&ResType, 0);
    self->response = response;
    self->code = NULL;
    self->headers = NULL;
    self->body = NULL;
    return (PyObject *)self;
}

void Res_dealloc(Res *self) {
    Py_XDECREF(self->code);
    Py_XDECREF(self->headers);
    Py_XDECREF(self->body);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Res_get_code(Res *self, void *closure) {
    if (!self->code) {
        self->code = PyLong_FromUnsignedLong((unsigned long)self->response->code);
    }
    Py_INCREF(self->code);
    return self->code;
}

int Res_set_code(Res *self, PyObject *value, void *closure) {
    Py_INCREF(value);
    self->code = value;
    self->response->code = PyLong_AsUnsignedLong(value);
    return 0;
}

PyObject *Res_get_headers(Res *self, void *closure) {
    if (!self->headers) {
        self->headers = (PyObject *)ResHeaders_new(&self->response->headers);
    }
    Py_INCREF(self->headers);
    return self->headers;
}

PyObject *Res_get_body(Res *self, void *closure) {
    if (self->response->body_len == 0) {
        Py_RETURN_NONE;
    }
    if (!self->body) {
        self->body = PyBytes_FromStringAndSize(self->response->body, self->response->body_len);
    }
    Py_INCREF(self->body);
    return self->body;
}

int Res_set_body(Res *self, PyObject *value, void *closure) {
    if (PyObject_IsInstance(value, &PyUnicode_Type)) {
        if (self->body != NULL) {
            Py_DECREF(self->body);
        }
        self->body = value;
        Py_INCREF(self->body);
        self->response->body = PyUnicode_AsUTF8AndSize(value, &self->response->body_len);
    } else if (PyObject_IsInstance(value, &PyBytes_Type)) {
        if (self->body != NULL) {
            Py_DECREF(self->body);
        }
        self->body = value;
        Py_INCREF(self->body);
        PyBytes_AsStringAndSize(value, &self->response->body, &self->response->body_len);
    }
    return 0;
}

PyObject *Res_empty(Res *self, PyObject *args, PyObject *kwargs) {
    self->response->code = 204;
    HeaderMap_set(&self->response->headers, "Content-Type", 12, "application/json", 16);
    self->response->body = NULL;
    self->response->body_len = 0;
    Py_RETURN_NONE;
}

PyObject *Res_text(Res *self, PyObject *args, PyObject *kwargs) {
    self->response->code = 200;
    HeaderMap_set(&self->response->headers, "Content-Type", 12, "text/plain", 10);
    char *text;
    PyArg_ParseTuple(args, "s", &text);
    self->response->body = text;
    self->response->body_len = strlen(text);
    Py_RETURN_NONE;
}

PyObject *Res_html(Res *self, PyObject *args, PyObject *kwargs) {
    self->response->code = 200;
    HeaderMap_set(&self->response->headers, "Content-Type", 12, "text/html", 9);
    char *html;
    PyArg_ParseTuple(args, "s", &html);
    self->response->body = html;
    self->response->body_len = strlen(html);
    Py_RETURN_NONE;
}

PyObject *Res_json(Res *self, PyObject *args, PyObject *kwargs) {
    self->response->code = 200;
    HeaderMap_set(&self->response->headers, "Content-Type", 12, "application/json", 16);
    PyObject *obj;
    PyArg_ParseTuple(args, "O", &obj);
    self->response->body = JSON_encode(obj, &self->response->body_len);
    self->response->body_len = strlen(self->response->body);
    Py_RETURN_NONE;
}

PyGetSetDef Res_getset[] = {
    {"code", (getter)Res_get_code, (setter)Res_set_code, NULL, NULL},
    {"headers", (getter)Res_get_headers, NULL, NULL, NULL},
    {"body", (getter)Res_get_body, (setter)Res_set_body, NULL, NULL},
    {NULL}
};

PyMethodDef Res_methods[] = {
    {"empty", (PyCFunction)Res_empty, METH_VARARGS | METH_KEYWORDS, ""},
    {"text", (PyCFunction)Res_text, METH_VARARGS | METH_KEYWORDS, ""},
    {"html", (PyCFunction)Res_html, METH_VARARGS | METH_KEYWORDS, ""},
    {"json", (PyCFunction)Res_json, METH_VARARGS | METH_KEYWORDS, ""},
    {NULL}
};

PyObject *Res_repr(Res *self) {
    char *repr = Response_repr(self->response, "Res", 0);
    PyObject *py_repr = PyUnicode_FromString(repr);
    free(repr);
    return py_repr;
}

PyTypeObject ResType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.Res",
    .tp_basicsize = sizeof(Res),
    .tp_dealloc = (destructor)Res_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = Res_getset,
    .tp_methods = Res_methods,
    .tp_repr = (reprfunc)Res_repr,
    .tp_str = (reprfunc)Res_repr
};
