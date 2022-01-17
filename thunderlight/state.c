#include "state.h"


PyTypeObject StateType;

PyObject *State_new(Duostate *duostate) {
    State *self = NULL;
    self = (State *)StateType.tp_alloc(&StateType, 0);
    self->duostate = duostate;
    return (PyObject *)self;
}

void State_dealloc(State *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *State_length(State *self, void *closure) {
    PyObject *value = PyLong_FromSize_t(self->duostate->len);
    Py_INCREF(value);
    return value;
}

int State_ass_subscript(PyObject *self, PyObject *key, PyObject *value) {
    Py_ssize_t key_size;
    const char *string_key = PyUnicode_AsUTF8AndSize(key, &key_size);
    Py_INCREF(value);
    Duostate_set_pyobject(((State *)self)->duostate, (char *)string_key, value);
    return 0;
}

PyObject *State_subscript(PyObject *self, PyObject *key) {
    const char *string_key = PyUnicode_AsUTF8(key);
    return Duostate_get_pyobject(((State *)self)->duostate, (char *)string_key);
}

PyMappingMethods State_mapping_methods = {
    .mp_length = State_length,
    .mp_subscript = State_subscript,
    .mp_ass_subscript = State_ass_subscript
};

PyTypeObject StateType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.State",
    .tp_basicsize = sizeof(State),
    .tp_dealloc = (destructor)State_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &State_mapping_methods
};
