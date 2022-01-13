#include "ctx.h"


static PyTypeObject CtxType;

Ctx *Ctx_new(Context *context) {
    Ctx *self = NULL;
    self = (Ctx *)CtxType.tp_alloc(&CtxType, 0);
    self->context = context;
    self->req = NULL;
    self->res = NULL;
    self->state = NULL;
    return self;
}

void Ctx_dealloc(Ctx *self) {
    Py_XDECREF(self->req);
    Py_XDECREF(self->res);
    Py_XDECREF(self->state);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Ctx_get_req(Ctx *self, void *closure) {
    if (!self->req) {
        self->req = (Req *)Req_new(self->context->request);
    }
    Py_INCREF(self->req);
    return (PyObject *)self->req;
}

static PyObject *Ctx_get_res(Ctx *self, void *closure) {
    if (!self->res) {
        self->res = (Res *)Res_new(self->context->response);
    }
    Py_INCREF(self->res);
    return (PyObject *)self->res;
}

static PyObject *Ctx_get_state(Ctx *self, void *closure) {
    if (!self->state) {
        self->state = (State *)State_new(self->context->duostate);
    }
    Py_INCREF(self->state);
    return (PyObject *)self->state;
}

static PyGetSetDef Ctx_getset[] = {
    {"req", (getter)Ctx_get_req, NULL, NULL, NULL},
    {"res", (getter)Ctx_get_res, NULL, NULL, NULL},
    {"state", (getter)Ctx_get_state, NULL, NULL, NULL},
    {NULL}
};

static PyTypeObject CtxType = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "Ctx",
    .tp_basicsize = sizeof(Ctx),
    .tp_dealloc = (destructor)Ctx_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = Ctx_getset,
    .tp_alloc = PyType_GenericAlloc
};
