#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <Python.h>
#include "hash.h"


#define DUOSTATE_INITIAL_CAPACITY 8

typedef PyObject *(*to_py_value)(void *);
typedef void *(*to_c_value)(PyObject *);

typedef struct {
    char *c_key;
    PyObject *py_key;
    void *c_value;
    PyObject *py_value;
    to_c_value to_c_value;
    to_py_value to_py_value;
    uint32_t key_hash;
    bool free_c_key;
    bool free_c_value;
} DuostateItem;

typedef struct {
    DuostateItem *buffer;
    DuostateItem inline_buffer[DUOSTATE_INITIAL_CAPACITY];
    size_t len;
    size_t capacity;
} Duostate;

Duostate *Duostate_new(void);

void Duostate_dealloc(Duostate *self);

void Duostate_init(Duostate *self);

void Duostate_deinit(Duostate *self);

void Duostate_set_c(Duostate *self, char *key, void *value, to_py_value converter, bool free_key, bool free_value);

void Duostate_set_py(Duostate *self, PyObject *key, PyObject *value, to_c_value converter);

void *Duostate_get_c(Duostate *self, char *key);

PyObject *Duostate_get_py(Duostate *self, PyObject *key);

char *DuostateItem_get_c_key(DuostateItem *self);

char *DuostateItem_get_py_value_repr(DuostateItem *self);

char *Duostate_repr(Duostate *self, char *head, uint8_t indent);

#ifdef __cplusplus
}
#endif
