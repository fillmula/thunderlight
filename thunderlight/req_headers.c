#include "req_headers.h"


PyTypeObject ReqHeadersType;

ReqHeaders *ReqHeaders_new(Request *request) {
    ReqHeaders *self = NULL;
    self = (ReqHeaders *)ReqHeadersType.tp_alloc(&ReqHeadersType, 0);
    self->request = request;
    return self;
}

void ReqHeaders_dealloc(ReqHeaders *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

Py_ssize_t ReqHeaders_length(PyObject *self) {
    return (Py_ssize_t)(((ReqHeaders *)self)->request->header_num);
}

PyObject *ReqHeaders_subscript(PyObject *self, PyObject *key) {
    const char *string_key = PyUnicode_AsUTF8(key);
    for (uint8_t i = 0; i < ((ReqHeaders *)self)->request->header_num; i++) {
        if (strcmp(((ReqHeaders *)self)->request->headers[i].name, string_key) == 0) {
            PyObject *value = PyUnicode_FromStringAndSize(
                ((ReqHeaders *)self)->request->headers[i].value,
                ((ReqHeaders *)self)->request->headers[i].value_len
            );
            Py_INCREF(value);
            return value;
        }
    }
    Py_RETURN_NONE;
}

PyObject *ReqHeaders_repr(ReqHeaders *self) {
    char *repr = Request_headers_repr(self->request, "ReqHeaders", 0);
    PyObject *py_repr = PyUnicode_FromString(repr);
    free(repr);
    return py_repr;
}

int ReqHeaders_contains(ReqHeaders *self, PyObject *item) {
    const char *string_key = PyUnicode_AsUTF8(item);
    for (uint8_t i = 0; i < ((ReqHeaders *)self)->request->header_num; i++) {
        if (strcmp(((ReqHeaders *)self)->request->headers[i].name, string_key) == 0) {
            return 1;
        }
    }
    return 0;
}

PyMappingMethods ReqHeaders_mapping_methods = {
    .mp_length = ReqHeaders_length,
    .mp_subscript = ReqHeaders_subscript,
    .mp_ass_subscript = NULL
};

PySequenceMethods ReqHeaders_sequence_methods = {
    .sq_contains = (objobjproc)ReqHeaders_contains
};

PyTypeObject ReqHeadersType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.ReqHeaders",
    .tp_basicsize = sizeof(ReqHeaders),
    .tp_dealloc = (destructor)ReqHeaders_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &ReqHeaders_mapping_methods,
    .tp_repr = (reprfunc)ReqHeaders_repr,
    .tp_str = (reprfunc)ReqHeaders_repr,
    .tp_as_sequence = &ReqHeaders_sequence_methods
};
