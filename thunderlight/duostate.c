#include "duostate.h"
#include "space.h"


void Duostate_init(Duostate *self) {
    self->capacity = DUOSTATE_INITIAL_CAPACITY;
    self->len = 0;
    self->buffer = self->inline_buffer;
}

Duostate *Duostate_new(void) {
    Duostate *self = (Duostate *)malloc(sizeof(Duostate));
    Duostate_init(self);
    return self;
}

void Duostate_deinit(Duostate *self) {
    for (size_t i = 0; i < self->len; i++) {
        Py_XDECREF(self->buffer[i].py_key);
        Py_DECREF(self->buffer[i].py_value);
        if (self->buffer[i].free_c_key) {
            free(self->buffer[i].c_key);
        }
        if (self->buffer[i].free_c_value) {
            free(self->buffer[i].c_value);
        }
    }
    if (self->buffer != self->inline_buffer) {
        free(self->buffer);
    }
}

void Duostate_dealloc(Duostate *self) {
    Duostate_deinit(self);
    free(self);
}

DuostateItem *_Duostate_get_pos(Duostate *self, uint32_t hash) {
    DuostateItem *pos = NULL;
    for (size_t i = 0; i < self->len; i++) {
        if (self->buffer[i].key_hash == hash) {
            pos = &(self->buffer[i]);
        }
    }
    return pos;
}

void _Duostate_clear_pos(DuostateItem *pos) {
    if (pos->free_c_key) {
        free(pos->c_key);
    }
    pos->c_key = NULL;
    if (pos->free_c_value) {
        free(pos->c_value);
    }
    pos->c_value = NULL;
    Py_XDECREF(pos->py_key);
    Py_XDECREF(pos->py_value);
    pos->to_c_value = NULL;
    pos->to_py_value = NULL;
    pos->free_c_key = false;
    pos->free_c_value = false;
}

void _Duostate_extend_if_needed(Duostate *self) {
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
}

void Duostate_set_c(Duostate *self, char *key, void *value, to_py_value converter, bool free_key, bool free_value) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = _Duostate_get_pos(self, hash);
    if (pos != NULL) {
        _Duostate_clear_pos(pos);
        pos->c_key = key;
        pos->c_value = value;
        pos->to_py_value = converter;
        pos->free_c_key = free_key;
        pos->free_c_value = free_value;
        return;
    }
    _Duostate_extend_if_needed(self);
    size_t index = self->len;
    self->len++;
    self->buffer[index].c_key = key;
    self->buffer[index].c_value = value;
    self->buffer[index].key_hash = hash;
    self->buffer[index].to_py_value = converter;
    self->buffer[index].to_c_value = NULL;
    self->buffer[index].py_key = NULL;
    self->buffer[index].py_value = NULL;
    self->buffer[index].free_c_key = free_key;
    self->buffer[index].free_c_value = free_value;
}

void Duostate_set_py(Duostate *self, PyObject *key, PyObject *value, to_c_value converter) {
    Py_ssize_t size;
    char *c_key = PyUnicode_AsUTF8AndSize(key, &size);
    uint32_t hash = fast_hash(c_key, (int)size);
    DuostateItem *pos = _Duostate_get_pos(self, hash);
    if (pos != NULL) {
        _Duostate_clear_pos(pos);
        pos->py_key = key;
        Py_INCREF(key);
        pos->py_value = value;
        Py_INCREF(value);
        pos->to_c_value = converter;
        return;
    }
    _Duostate_extend_if_needed(self);
    size_t index = self->len;
    self->len++;
    self->buffer[index].c_key = NULL;
    self->buffer[index].c_value = NULL;
    self->buffer[index].key_hash = hash;
    self->buffer[index].to_py_value = NULL;
    self->buffer[index].to_c_value = converter;
    Py_INCREF(key);
    self->buffer[index].py_key = key;
    Py_INCREF(value);
    self->buffer[index].py_value = value;
    self->buffer[index].free_c_key = NULL;
    self->buffer[index].free_c_value = NULL;
}

void *Duostate_get_c(Duostate *self, char *key) {
    uint32_t hash = fast_hash(key, (int)strlen(key));
    DuostateItem *pos = _Duostate_get_pos(self, hash);
    if (pos == NULL) {
        return NULL;
    }
    if (pos->c_value != NULL) {
        return pos->c_value;
    }
    if (pos->to_c_value != NULL) {
        pos->c_value = pos->to_c_value(pos->py_value);
        return pos->c_value;
    }
    return NULL;
}

PyObject *Duostate_get_py(Duostate *self, PyObject *key) {
    Py_ssize_t size;
    char *c_key = PyUnicode_AsUTF8AndSize(key, &size);
    uint32_t hash = fast_hash(c_key, (int)size);
    DuostateItem *pos = _Duostate_get_pos(self, hash);
    if (pos == NULL) {
        Py_RETURN_NONE;
    }
    if (pos->py_value != NULL) {
        Py_INCREF(pos->py_value);
        return pos->py_value;
    }
    if (pos->to_py_value != NULL) {
        pos->py_value = pos->to_py_value(pos->c_value);
        Py_INCREF(pos->py_value);
        return pos->py_value;
    }
    Py_RETURN_NONE;
}

char *DuostateItem_get_c_key(DuostateItem *self) {
    if (self->c_key == NULL) {
        self->c_key = (char *)PyUnicode_AsUTF8(self->py_key);
    }
    return self->c_key;
}

char *DuostateItem_get_py_value_repr(DuostateItem *self) {
    if (self->py_value == NULL) {
        self->py_value = self->to_py_value(self->c_value);
        Py_INCREF(self->py_value);
    }
    PyObject *repr = PyObject_Repr(self->py_value);
    return (char *)PyUnicode_AsUTF8(repr);
}

char *Duostate_repr(Duostate *self, char *head, uint8_t indent) {
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    if (self->len == 0) {
        strcat(buffer, "{}");
        return buffer;
    }
    strcat(buffer, "{\n");
    for (size_t i = 0; i < self->len; i++) {
        if (i != 0) {
            strcat(buffer, ",\n");
        }
        add_space(buffer, (indent + 1) * 4);
        strcat(buffer, "'");
        strcat(buffer, DuostateItem_get_c_key(&self->buffer[i]));
        strcat(buffer, "'");
        strcat(buffer, ": ");
        strcat(buffer, DuostateItem_get_py_value_repr(&self->buffer[i]));
    }
    strcat(buffer, "\n");
    add_space(buffer, indent * 4);
    strcat(buffer, "}");
    return buffer;
}
