#include "req_match.h"


PyTypeObject ReqMatchType;


ReqMatch *ReqMatch_new(MatchResult *mresult) {
    ReqMatch *self = (ReqMatch *)ReqMatchType.tp_alloc(&ReqMatchType, 0);
    self->mresult = mresult;
    return self;
}

PyObject *ReqMatch_repr(ReqMatch *self) {
    char *repr = MatchResult_repr(self->mresult, "ReqMatch", 0);
    PyObject *py_repr = PyUnicode_FromString(repr);
    free(repr);
    return py_repr;
}

void ReqMatch_dealloc(ReqMatch *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

Py_ssize_t ReqMatch_length(ReqMatch *self) {
    return (Py_ssize_t)self->mresult->len;
}

PyObject *ReqMatch_subscript(ReqMatch *self, PyObject *key) {
    const char *string_key = PyUnicode_AsUTF8(key);
    char *value = MatchResult_get(self->mresult, string_key);
    if (value == NULL) {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString((const char *)value);
}

PyMappingMethods ReqMatch_mapping_methods = {
    .mp_length = (lenfunc)ReqMatch_length,
    .mp_ass_subscript = NULL,
    .mp_subscript = (binaryfunc)ReqMatch_subscript
};

PyTypeObject ReqMatchType = {
    PyObject_HEAD_INIT(NULL)
    .tp_alloc = PyType_GenericAlloc,
    .tp_basicsize = sizeof(ReqMatch),
    .tp_doc = "",
    .tp_name = "thunderlight.ReqMatch",
    .tp_dealloc = (destructor)ReqMatch_dealloc,
    .tp_repr = (reprfunc)ReqMatch_repr,
    .tp_str = (reprfunc)ReqMatch_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &ReqMatch_mapping_methods,
};
