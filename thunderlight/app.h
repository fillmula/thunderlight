#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "matcher.h"
#include "context.h"
#include "middleware_chain.h"


typedef struct {
    PyObject_HEAD
    MatcherList *gets;
    MatcherList *posts;
    MatcherList *patches;
    MatcherList *deletes;
    PyObject *middlewares;
    PyObject *entrance_middleware;
} App;

static PyObject *App_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

int App_init(App* self, PyObject *args, PyObject *kwds);

void App_get(App *self, const char *route, PyObject *handle);

void App_post(App *self, const char *route, PyObject *handle);

void App_patch(App *self, const char *route, PyObject *handle);

void App_delete(App *self, const char *route, PyObject *handle);

void App_use(App *self, PyObject *middleware);

void App_prepare(App *self);

void App_process(App *self, PyObject *p);

#ifdef __cplusplus
}
#endif
