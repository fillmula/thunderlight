#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "app.h"
#include "request.h"
#include "response.h"
#include "res.h"
#include "ctx.h"


typedef struct {
    PyObject_HEAD
    App *app;
    PyObject *transport;
    Ctx *ctx;
    Request request;
    Response response;
    Duostate duostate;
    Context context;
} Protocol;

static PyTypeObject ProtocolType;

void Protocol_complete(Protocol *self);

// Protocol *Protocol_native_new(App *app);

#ifdef __cplusplus
}
#endif
