#include "req.h"
#include "req_headers.h"
#include "req_match.h"
#include "json.h"


PyTypeObject ReqType;

PyObject *Req_new(Request *request) {
    Req *self = NULL;
    self = (Req *)ReqType.tp_alloc(&ReqType, 0);
    self->request = request;
    self->args = NULL;
    self->method = NULL;
    self->path = NULL;
    self->query = NULL;
    self->version = NULL;
    self->json = NULL;
    self->body = NULL;
    self->headers = NULL;
    return (PyObject *)self;
}

void Req_dealloc(Req *self) {
    Py_XDECREF(self->args);
    Py_XDECREF(self->method);
    Py_XDECREF(self->path);
    Py_XDECREF(self->query);
    Py_XDECREF(self->version);
    Py_XDECREF(self->json);
    Py_XDECREF(self->body);
    Py_XDECREF(self->headers);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *Req_get_args(Req *self, void *closure) {
    if (!self->args) {
        if (self->request->mresult == NULL) {
            self->args = Py_None;
        } else {
            self->args = ReqMatch_new(self->request->mresult);
        }
    }
    Py_INCREF(self->args);
    return self->args;
}

PyObject *Req_get_method(Req *self, void *closure) {
    if (!self->method) {
        self->method = PyUnicode_DecodeLatin1(self->request->method,
                                              self->request->method_len,
                                              NULL);
    }
    Py_INCREF(self->method);
    return self->method;
}

PyObject *Req_get_path(Req *self, void *closure) {
    if (!self->path) {
        self->path = PyUnicode_DecodeLatin1(self->request->path,
                                            self->request->path_len,
                                            NULL);
    }
    Py_INCREF(self->path);
    return self->path;
}

PyObject *Req_get_query(Req *self, void *closure) {
    if (self->request->query_len == 0) {
        Py_RETURN_NONE;
    }
    if (!self->query) {
        self->query = PyUnicode_DecodeLatin1(self->request->query,
                                             self->request->query_len,
                                             NULL);
    }
    Py_INCREF(self->query);
    return self->query;
}

PyObject *Req_get_version(Req *self, void *closure) {
    if (!self->version) {
        self->version = PyUnicode_DecodeLatin1(self->request->version,
                                               self->request->version_len,
                                               NULL);
    }
    Py_INCREF(self->version);
    return self->version;
}

PyObject *Req_get_headers(Req *self, void *closure) {
    if (!self->headers) {
        self->headers = (PyObject *)ReqHeaders_new(self->request);
    }
    Py_INCREF(self->headers);
    return self->headers;
}

PyObject *Req_get_body(Req *self, void *closure) {
    if (!self->body) {
        if (self->request->body_len == 0) {
            self->body = PyBytes_FromString("");
        } else {
            self->body = PyBytes_FromStringAndSize(self->request->body, self->request->body_len);
        }
    }
    Py_INCREF(self->body);
    return self->body;
}

PyObject *Req_get_json(Req *self, void *closure) {
    if (!self->json) {
        PyObject *body = Req_get_body(self, NULL);
        self->json = JSON_decode(body);
    }
    Py_INCREF(self->json);
    return self->json;
}

PyGetSetDef Req_getset[] = {
    {"args", (getter)Req_get_args, NULL, NULL, NULL},
    {"method", (getter)Req_get_method, NULL, NULL, NULL},
    {"path", (getter)Req_get_path, NULL, NULL, NULL},
    {"query", (getter)Req_get_query, NULL, NULL, NULL},
    {"version", (getter)Req_get_version, NULL, NULL, NULL},
    {"headers", (getter)Req_get_headers, NULL, NULL, NULL},
    {"body", (getter)Req_get_body, NULL, NULL, NULL},
    {"json", (getter)Req_get_json, NULL, NULL, NULL},
    {NULL}
};

PyObject *Req_repr(Req *self) {
    char *repr = Request_repr(self->request, "Req", 0);
    PyObject *py_repr = PyUnicode_FromString(repr);
    free(repr);
    return py_repr;
}

PyTypeObject ReqType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.Req",
    .tp_basicsize = sizeof(Req),
    .tp_dealloc = (destructor)Req_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = Req_getset,
    .tp_repr = (reprfunc)Req_repr,
    .tp_str = (reprfunc)Req_repr
};
