#include "json.h"


PyObject *orjson = NULL;

PyObject *ujson = NULL;

PyObject *pyjson = NULL;

typedef char *(*encodefunc)(PyObject *obj, size_t *len);

typedef PyObject *(*decodefunc)(PyObject *buffer);

encodefunc encoder;

decodefunc decoder;


char *orjson_encode(PyObject *obj, size_t *len) {
    PyObject *dumps = PyObject_GetAttrString(orjson, "dumps");
    PyObject *bytes = PyObject_CallOneArg(dumps, obj);
    char *retval;
    PyBytes_AsStringAndSize(bytes, &retval, &len);
    return retval;
}

char *ujson_encode(PyObject *obj, size_t *len) {
    PyObject *dumps = PyObject_GetAttrString(ujson, "dumps");
    PyObject *str = PyObject_CallOneArg(dumps, obj);
    return PyUnicode_AsUTF8AndSize(str, &len);
}

char *pyjson_encode(PyObject *obj, size_t *len) {
    PyObject *dumps = PyObject_GetAttrString(pyjson, "dumps");
    PyObject *str = PyObject_CallOneArg(dumps, obj);
    return PyUnicode_AsUTF8AndSize(str, &len);
}

PyObject *orjson_decode(PyObject *buffer) {
    PyObject *loads = PyObject_GetAttrString(orjson, "loads");
    return PyObject_CallOneArg(loads, buffer);
}

PyObject *ujson_decode(PyObject *buffer) {
    PyObject *loads = PyObject_GetAttrString(ujson, "loads");
    return PyObject_CallOneArg(loads, buffer);
}

PyObject *pyjson_decode(PyObject *buffer) {
    PyObject *loads = PyObject_GetAttrString(pyjson, "loads");
    return PyObject_CallOneArg(loads, buffer);
}

void JSON_setup() {
    orjson = PyImport_ImportModule("orjson");
    if (orjson == NULL) {
        PyErr_Clear();
        ujson = PyImport_ImportModule("ujson");
        if (ujson == NULL) {
            PyErr_Clear();
            pyjson = PyImport_ImportModule("json");
        }
    }
    if (orjson != NULL) {
        encoder = orjson_encode;
        decoder = orjson_decode;
    } else if (ujson != NULL) {
        encoder = ujson_encode;
        decoder = ujson_decode;
    } else {
        encoder = pyjson_encode;
        decoder = pyjson_decode;
    }
}

char *JSON_encode(PyObject *obj, size_t *len) {
    return encoder(obj, &len);
}

PyObject *JSON_decode(PyObject *buffer) {
    return decoder(buffer);
}
