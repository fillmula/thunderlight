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
    PyObject *route;
    MatcherList *mlist;
} RouteWrapper;

typedef struct {
    PyObject_HEAD
    MatcherList *gets;
    MatcherList *posts;
    MatcherList *patches;
    MatcherList *deletes;
    PyObject *middlewares;
    PyObject *entrance_middleware;
} App;

extern PyTypeObject AppType;

extern PyTypeObject RouteWrapperType;

PyObject *App_native_new();

int App_init(App* self, PyObject *args, PyObject *kwds);

void App_get(App *self, const char *route, PyObject *handle);

void App_post(App *self, const char *route, PyObject *handle);

void App_patch(App *self, const char *route, PyObject *handle);

void App_delete(App *self, const char *route, PyObject *handle);

void App_use(App *self, PyObject *middleware);

void App_prepare(App *self);

void App_process(App *self, PyObject *p);

PyObject *App_get_wrapper(App *self, PyObject *route);

PyObject *App_post_wrapper(App *self, PyObject *route);

PyObject *App_patch_wrapper(App *self, PyObject *route);

PyObject *App_delete_wrapper(App *self, PyObject *route);

#ifdef __cplusplus
}
#endif
