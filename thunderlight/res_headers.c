#include "res_headers.h"


static PyTypeObject ResHeadersType;

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

static PyMappingMethods ResHeaders_mapping_methods = {
    .mp_length = ResHeaders_length,
    .mp_subscript = ResHeaders_subscript,
    .mp_ass_subscript = ResHeaders_ass_subscript
};

static PyTypeObject ResHeadersType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "ResHeaders",
    .tp_basicsize = sizeof(ResHeaders),
    .tp_dealloc = (destructor)ResHeaders_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &ResHeaders_mapping_methods
};

static PyModuleDef ResHeaders_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "res_headers",
    .m_doc = "res_headers",
    .m_size = -1
};

PyMODINIT_FUNC
PyInit_res_headers(void) {
    PyObject* m = NULL;
    if (PyType_Ready(&ResHeadersType) < 0) {
        goto error;
    }
    m = PyModule_Create(&ResHeaders_module);
    Py_INCREF(&ResHeadersType);
    PyModule_AddObject(m, "ResHeaders", (PyObject *)&ResHeadersType);
    if (!m) {
        goto error;
    }
    goto finally;
error:
    m = NULL;
finally:
    return m;
}
