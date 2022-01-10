#include "duostate.h"
#include "hash.h"


Duostate *Duostate_alloc(void) {
    return (Duostate *)malloc(sizeof(Duostate));
}

void Duostate_init(Duostate *self) {
    self->len = 0;
    self->capacity = DUOSTATE_INITIAL_CAPACITY;
    self->buffer = self->inline_buffer;
}

void Duostate_dealloc(Duostate *self) {
    if (self->buffer != self->inline_buffer) {
        free(self->buffer);
    }
    free(self);
    // TODO: memory leak for items
}

void Duostate_set_native(Duostate *self, char *key, void *value, to_py_value converter) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    if (pos != NULL) {
        free(pos->value);
        Py_XDECREF(pos->py_value);
        pos->value = value;
        pos->py_value = NULL;
        pos->to_py_value = converter;
    } else {
        if (self->len == self->capacity) {
            size_t copy_size = self->capacity * sizeof(DuostateItem);
            self->capacity = self->capacity + DUOSTATE_INITIAL_CAPACITY;
            if (self->buffer == self->inline_buffer) {
                self->buffer = malloc(self->capacity * sizeof(DuostateItem));
                memcpy(self->buffer, self->inline_buffer, copy_size);
            } else {
                self->buffer = realloc(self->buffer, self->capacity * sizeof(DuostateItem));
            }
        }
        size_t index = self->len;
        self->len++;
        self->buffer[index].value = value;
        self->buffer[index].key_hash = hash;
        self->buffer[index].to_py_value = converter;
        self->buffer[index].py_value = NULL;
    }
}

void Duostate_set_pyobject(Duostate *self, char *key, PyObject *value) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    if (pos != NULL) {
        free(pos->value);
        Py_XDECREF(pos->py_value);
        pos->value = NULL;
        Py_INCREF(value);
        pos->py_value = value;
        pos->to_py_value = NULL;
    } else {
        if (self->len == self->capacity) {
            size_t copy_size = self->capacity * sizeof(DuostateItem);
            self->capacity = self->capacity + DUOSTATE_INITIAL_CAPACITY;
            if (self->buffer == self->inline_buffer) {
                self->buffer = malloc(self->capacity * sizeof(DuostateItem));
                memcpy(self->buffer, self->inline_buffer, copy_size);
            } else {
                self->buffer = realloc(self->buffer, self->capacity * sizeof(DuostateItem));
            }
        }
        size_t index = self->len;
        self->len++;
        self->buffer[index].value = NULL;
        self->buffer[index].key_hash = hash;
        self->buffer[index].to_py_value = NULL;
        self->buffer[index].py_value = value;
    }
}

void *Duostate_get_native(Duostate *self, char *key) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    if (pos == NULL) {
        return NULL;
    } else {
        return pos->value;
    }
}

PyObject *Duostate_get_pyobject(Duostate *self, char *key) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    if (pos == NULL) {
        Py_RETURN_NONE;
    } else {
        if (pos->py_value != NULL) {
            Py_INCREF(pos->py_value);
            return pos->py_value;
        } else {
            PyObject *pyval = pos->to_py_value(pos->value);
            Py_INCREF(pyval);
            pos->py_value = pyval;
            return pyval;
        }
    }
}
