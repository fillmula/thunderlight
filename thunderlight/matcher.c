#include "matcher.h"
#include "not_found.h"


MatcherList *MatcherList_alloc(void) {
    MatcherList *self = malloc(sizeof(MatcherList));
    return self;
}

void MatcherList_init(MatcherList *self) {
    self->capacity = MATCHER_LIST_INITIAL_SIZE;
    self->length = 0;
    self->buffer = self->inline_buffer;
}

void MatcherList_dealloc(MatcherList *self) {
    for (size_t i = 0; i < self->length; i++) {
        Py_DECREF(self->buffer[i].handler);
    }
    if (self->buffer != self->inline_buffer) {
        free(self->buffer);
    }
    free(self);
}

void Matcher_record(Matcher *self) {
    char *pos = (char *)self->route;
    self->is_static = true;
    self->ele_num = 0;
    bool next_ele = false;
    while (*pos != '\0') {
        if (*pos == ':') {
            self->is_static = false;
        }
        if (*pos == '/') {
            next_ele = true;
        }
        if (next_ele) {
            self->ele_num++;
            next_ele = false;
        }
        pos++;
    }
    self->len = pos - self->route;
}

void MatcherList_append(MatcherList *self, const char *route, PyObject *handler) {
    size_t index = self->length;
    if (index > self->capacity) { // TODO: unlikely
        self->capacity += 32;
        if (self->buffer == self->inline_buffer) {
            self->buffer = malloc(self->capacity * sizeof(Matcher));
            memcpy(self->buffer, self->inline_buffer, MATCHER_LIST_INITIAL_SIZE * sizeof(Matcher));
        } else {
            self->buffer = realloc(self->buffer, self->capacity * sizeof(Matcher));
        }
    }
    Py_INCREF(handler);
    self->buffer[index].handler = handler;
    self->buffer[index].route = route;
    Matcher_record(&self->buffer[index]);
    self->length++;
}

bool Matcher_match(Matcher *self, char *path) {
    size_t len = strlen(path);
    if (self->is_static) {
        if (len != self->len) {
            return false;
        }
        if (strcmp(path, self->route) == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

PyObject *MatcherList_match(MatcherList *self, char *path) {
    for (size_t i = 0; i < self->length; i++) {
        if (Matcher_match(&self->buffer[i], path)) {
            Py_INCREF(self->buffer[i].handler);
            return self->buffer[i].handler;
        }
    }
    Py_INCREF(not_found);
    return not_found;
}
