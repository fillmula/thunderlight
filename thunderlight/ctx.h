#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "context.h"
#include "req.h"
#include "res.h"
#include "state.h"


typedef struct {
    PyObject_HEAD
    Context *context;
    Req *req;
    Res *res;
    State *state;
} Ctx;

static PyTypeObject CtxType;

Ctx *Ctx_new(Context *context);

void Ctx_dealloc(Ctx *self);

#ifdef __cplusplus
}
#endif
