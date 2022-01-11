#include <Python.h>
#include "app.h"
#include "protocol.h"


static PyTypeObject RouteWrapperType;

RouteWrapper *RouteWrapper_new(MatcherList *mlist, PyObject *route) {
    RouteWrapper *self = (RouteWrapper *)RouteWrapperType.tp_alloc(&RouteWrapperType, 0);
    self->mlist = mlist;
    self->route = route;
    Py_INCREF(self->route);
    return self;
}

void RouteWrapper_dealloc(RouteWrapper *self) {
    Py_DECREF(self->route);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *RouteWrapper_call(RouteWrapper *self, PyObject *args, PyObject *kwds) {
    PyObject *handle;
    PyArg_Parse(args, "O", &handle);
    char *route = PyUnicode_AsUTF8(self->route);
    MatcherList_append(self->mlist, (const char *)route, handle);
    Py_RETURN_NONE;
}

static PyTypeObject RouteWrapperType = {
    .tp_name = "RouteWrapper",
    .tp_doc = "RouteWrapper",
    .tp_itemsize = sizeof(RouteWrapper),
    .tp_call = (ternaryfunc)RouteWrapper_call,
    .tp_dealloc = (destructor)RouteWrapper_dealloc
};

int App_init(App *self, PyObject *args, PyObject *kwds) {
    self->gets = MatcherList_alloc();
    MatcherList_init(self->gets);
    self->posts = MatcherList_alloc();
    MatcherList_init(self->posts);
    self->patches = MatcherList_alloc();
    MatcherList_init(self->patches);
    self->deletes = MatcherList_alloc();
    self->middlewares = PyList_New(16);
    self->entrance_middleware = NULL;
    return 0;
}

void App_dealloc(App *self) {
    MatcherList_dealloc(self->gets);
    MatcherList_dealloc(self->posts);
    MatcherList_dealloc(self->patches);
    MatcherList_dealloc(self->deletes);
    Py_DECREF(self->middlewares);
    Py_XDECREF(self->entrance_middleware);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

void _App_route(App *self, const char *route, PyObject *handle, MatcherList *matcher_list) {
    MatcherList_append(matcher_list, route, handle);
}

void App_get(App *self, const char *route, PyObject *handle) {
    _App_route(self, route, handle, self->gets);
}

void App_post(App *self, const char *route, PyObject *handle) {
    _App_route(self, route, handle, self->posts);
}

void App_patch(App *self, const char *route, PyObject *handle) {
    _App_route(self, route, handle, self->patches);
}

void App_delete(App *self, const char *route, PyObject *handle) {
    _App_route(self, route, handle, self->deletes);
}

void App_use(App *self, PyObject *middleware) {
    PyList_Append(self->middlewares, middleware);
}

void App_prepare(App *self) {
    self->entrance_middleware = ChainedMiddleware_build(self->middlewares);
    Py_INCREF(self->entrance_middleware);
}

void App_process(App *self, PyObject *p) {
    Protocol *protocol = (Protocol *)p;
    MatcherList *mlist = NULL;
    switch (protocol->request.method_len) {
        case 3:
            mlist = self->gets;
            break;
        case 4:
            mlist = self->posts;
            break;
        case 5:
            mlist = self->patches;
            break;
        case 6:
            mlist = self->deletes;
            break;
    }
    PyObject *handler = MatcherList_match(mlist, protocol->request.path);
    PyObject *call_args = PyTuple_New(2);
    PyTuple_SetItem(call_args, 0, (PyObject *)protocol->ctx);
    PyTuple_SetItem(call_args, 1, handler);
    PyObject *awaitable = PyObject_Call(self->entrance_middleware, call_args, NULL);
    Py_DECREF(call_args);
    PyObject *asyncio = PyImport_ImportModule("asyncio");
    PyObject *ensure_future = PyObject_GetAttrString(asyncio, "ensure_future");
    PyObject *future = PyObject_CallOneArg(ensure_future, awaitable);
    PyObject *add_done_callback = PyObject_GetAttrString(future, "add_done_callback");
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, (PyObject *)protocol);
    PyObject_Call(add_done_callback, args, NULL);
}

static PyObject *App_python_get(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->gets, route);
}

static PyObject *App_python_post(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->posts, route);
}

static PyObject *App_python_patch(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->patches, route);
}

static PyObject *App_python_delete(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->deletes, route);
}

static PyObject *App_python_use(App *self, PyObject *middleware) {
    App_use(self, middleware);
    Py_RETURN_NONE;
}


static PyMethodDef App_methods[] = {
    {"get", (PyCFunction)App_python_get, METH_O, NULL},
    {"post", (PyCFunction)App_python_post, METH_O, NULL},
    {"patch", (PyCFunction)App_python_patch, METH_O, NULL},
    {"delete", (PyCFunction)App_python_delete, METH_O, NULL},
    {"use", (PyCFunction)App_python_use, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject AppType = {
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)App_init,
    .tp_dealloc = (destructor)App_dealloc,
    .tp_doc = "App",
    .tp_name = "App",
    .tp_basicsize = sizeof(App),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = App_methods
};

static PyModuleDef App_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "app",
    .m_doc = "app",
    .m_size = -1
};

PyMODINIT_FUNC PyInit_app(void) {
    if (PyType_Ready(&RouteWrapperType) < 0) {
        return NULL;
    }
    if (PyType_Ready(&AppType) < 0) {
        return NULL;
    }
    PyObject *module = PyModule_Create(&App_module);
    PyModule_AddType(module, &AppType);
    PyModule_AddType(module, &RouteWrapperType);
    return module;
}
