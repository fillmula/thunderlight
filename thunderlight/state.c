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

PyObject *State_repr(State *self) {
    char *headers = malloc(255);
    headers[0] = '\0';
    strcat(headers, "State {\n");
    for (size_t i = 0; i < self->duostate->len; i++) {
        if (i != 0) {
            strcat(headers, ",\n");
        }
        strcat(headers, "    \"");
        strcat(headers, DuostateItem_get_c_key(&self->duostate->buffer[i]));
        strcat(headers, "\"");
        strcat(headers, ": ");
        strcat(headers, "\"");
        strcat(headers, DuostateItem_get_py_value_repr(&self->duostate->buffer[i]));
        strcat(headers, "\"");
    }
    strcat(headers, "\n}");
    return headers;
}

PyObject *State_length(State *self, void *closure) {
    PyObject *value = PyLong_FromSize_t(self->duostate->len);
    Py_INCREF(value);
    return value;
}

int State_ass_subscript(State *self, PyObject *key, PyObject *value) {
    Duostate_set_py(self->duostate, key, value, NULL);
    return 0;
}

PyObject *State_subscript(State *self, PyObject *key) {
    return Duostate_get_py(self->duostate, key);
}

PyMappingMethods State_mapping_methods = {
    .mp_length = State_length,
    .mp_subscript = (binaryfunc)State_subscript,
    .mp_ass_subscript = (objobjargproc)State_ass_subscript
};

PyTypeObject StateType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "thunderlight.State",
    .tp_basicsize = sizeof(State),
    .tp_dealloc = (destructor)State_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_as_mapping = &State_mapping_methods,
    .tp_repr = (reprfunc)State_repr,
    .tp_str = (reprfunc)State_repr
};
