#pragma once

#include <Python.h>
#include <stdbool.h>


typedef struct {
    PyObject_HEAD
    PyObject *inner;
    PyObject *next;
} OuterNext;

typedef struct {
    PyObject_HEAD
    PyObject *outer;
    PyObject *inner;
} ChainedMiddleware;

typedef struct {
    PyObject_HEAD
    OuterNext *outer_next;
    PyObject *ctx;
    uint8_t state;
} OuterNextIterator;

typedef struct {
    PyObject_HEAD
    ChainedMiddleware *chained_middleware;
    PyObject *ctx;
    PyObject *next;
    uint8_t state;
} ChainedMiddlewareIterator;

extern PyTypeObject ChainedMiddlewareIteratorType;
extern PyTypeObject ChainedMiddlewareType;
extern PyTypeObject OuterNextIteratorType;
extern PyTypeObject OuterNextType;

PyObject *ChainedMiddleware_build(PyObject *list);
