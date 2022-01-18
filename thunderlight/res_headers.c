#include "res_headers.h"


PyTypeObject ResHeadersType;

ResHeaders *ResHeaders_new(HeaderMap *header_map) {
    ResHeaders *self = NULL;
    self = (ResHeaders *)ResHeadersType.tp_alloc(&ResHeadersType, 0);
    self->header_map = header_map;
    return self;
}

void ResHeaders_dealloc(ResHeaders *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

Py_ssize_t ResHeaders_length(PyObject *self) {
    return (Py_ssize_t)(((ResHeaders *)self)->header_map->len);
}

int ResHeaders_ass_subscript(PyObject *self, PyObject *key, PyObject *value) {
    Py_ssize_t key_size;
    const char *string_key = PyUnicode_AsUTF8AndSize(key, &key_size);
    Py_ssize_t value_size;
    const char *string_value = PyUnicode_AsUTF8AndSize(value, &value_size);
    char *copied_key = malloc(key_size);
    strcpy(copied_key, string_key);
    char *copied_value = malloc(value_size);
    strcpy(copied_value, string_value);
    HeaderMap_set(((ResHeaders *)self)->header_map, copied_key, key_size, copied_value, value_size);
    return 0;
}

PyObject *ResHeaders_subscript(PyObject *self, PyObject *key) {
    const char *string_key = PyUnicode_AsUTF8(key);
    char *value = HeaderMap_get(((ResHeaders *)self)->header_map, (char *)string_key);
    if (value == NULL) {
        Py_RETURN_NONE;
    } else {
        PyObject *str = PyUnicode_FromStringAndSize(value, strlen(value));
        Py_INCREF(str);
        return str;
    }
}

PyObject *ResHeaders_repr(ResHeaders *self) {
    char *repr = HeaderMap_repr(self->header_map, "ResHeaders", 0);
    PyObject *py_repr = PyUnicode_FromString(repr);
    free(repr);
    return py_repr;
}

PyMappingMethods ResHeaders_mapping_methods = {
    .mp_length = ResHeaders_length,
    .mp_subscript = ResHeaders_subscript,
    .mp_ass_subscript = ResHeaders_ass_subscript
};

PyTypeObject ResHeadersType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.ResHeaders",
    .tp_basicsize = sizeof(ResHeaders),
    .tp_dealloc = (destructor)ResHeaders_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &ResHeaders_mapping_methods,
    .tp_repr = (reprfunc)ResHeaders_repr,
    .tp_str = (reprfunc)ResHeaders_repr
};
