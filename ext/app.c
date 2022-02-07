#include <Python.h>
#include "app.h"
#include "protocol.h"
#include "loop.h"


PyTypeObject RouteWrapperType;

RouteWrapper *RouteWrapper_new(MatcherList *mlist, PyObject *route) {
    RouteWrapper *self = (RouteWrapper *)RouteWrapperType.tp_alloc(&RouteWrapperType, 0);
    self->mlist = mlist;
    self->route = route;
    Py_INCREF(self->route);
    return self;
}

void RouteWrapper_dealloc(RouteWrapper *self) {
    Py_XDECREF(self->route);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *RouteWrapper_call(RouteWrapper *self, PyObject *args, PyObject *kwds) {
    PyObject *handle;
    PyArg_ParseTuple(args, "O", &handle);
    const char *route = PyUnicode_AsUTF8(self->route);
    Py_INCREF(self->route);
    MatcherList_append(self->mlist, route, handle);
    Py_RETURN_NONE;
}

PyTypeObject RouteWrapperType = {
    .tp_name = "_thunderlight._RouteWrapper",
    .tp_doc = "RouteWrapper",
    .tp_basicsize = sizeof(RouteWrapper),
    .tp_call = (ternaryfunc)RouteWrapper_call,
    .tp_dealloc = (destructor)RouteWrapper_dealloc
};

int App_init(App *self, PyObject *args, PyObject *kwds) {
    self->gets = MatcherList_new("GET");
    self->posts = MatcherList_new("POST");
    self->patches = MatcherList_new("PATCH");
    self->deletes = MatcherList_new("DELETE");
    self->middlewares = PyList_New(0);
    self->entrance_middleware = Py_None;
    Py_INCREF(Py_None);
    self->prepares = PyList_New(0);
    self->prepare = Py_None;
    Py_INCREF(Py_None);
    return 0;
}

PyObject *App_native_new() {
    App *app = AppType.tp_alloc(&AppType, 0);
    App_init(app, NULL, NULL);
    return app;
}

void App_dealloc(App *self) {
    MatcherList_dealloc(self->gets);
    MatcherList_dealloc(self->posts);
    MatcherList_dealloc(self->patches);
    MatcherList_dealloc(self->deletes);
    Py_XDECREF(self->middlewares);
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

// void App_prepare(App *self) {
//     self->entrance_middleware = ChainedMiddleware_build(self->middlewares);
//     Py_XINCREF(self->entrance_middleware);
//     Py_ssize_t prepares_len = PyList_Size(self->prepares);
//     for (Py_ssize_t i = 0; i < prepares_len; i++) {
//         PyObject_CallNoArgs(PyList_GetItem(self->prepares, i));
//     }
// }

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

    PyObject *handler = MatcherList_match(mlist, protocol->request.path, &protocol->request);
    PyObject *awaitable;
    if (Py_None == self->entrance_middleware) {
        PyObject *call_args = PyTuple_New(1);
        PyTuple_SetItem(call_args, 0, (PyObject *)protocol->ctx);
        Py_INCREF(protocol->ctx);
        awaitable = PyObject_Call(handler, call_args, NULL);
        Py_INCREF(awaitable);
        Py_INCREF(handler);
        Py_XDECREF(call_args);
    } else {
        PyObject *call_args = PyTuple_New(2);
        PyTuple_SetItem(call_args, 0, (PyObject *)protocol->ctx);
        Py_INCREF(protocol->ctx);
        PyTuple_SetItem(call_args, 1, handler);
        awaitable = PyObject_Call(self->entrance_middleware, call_args, NULL);
        Py_INCREF(awaitable);
        Py_INCREF(handler);
        Py_XDECREF(call_args);
    }
    Py_INCREF(awaitable);
    PyObject *future = Loop_start_awaitable(awaitable);
    Py_INCREF(future);
    PyObject *add_done_callback = PyObject_GetAttrString(future, "add_done_callback");
    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, (PyObject *)protocol);
    PyObject_Call(add_done_callback, args, NULL);
    Py_XDECREF(args);
}

PyObject *App_python_get(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->gets, route);
}

PyObject *App_python_post(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->posts, route);
}

PyObject *App_python_patch(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->patches, route);
}

PyObject *App_python_delete(App *self, PyObject *route) {
    return (PyObject *)RouteWrapper_new(self->deletes, route);
}

PyObject *App_python_use(App *self, PyObject *middleware) {
    App_use(self, middleware);
    Py_RETURN_NONE;
}

PyObject *App_on_prepare(App *self, PyObject *callable) {
    PyList_Append(self->prepares, callable);
    Py_RETURN_NONE;
}

PyObject *App_sett_prepare(App *self, PyObject *callable) {
    Py_INCREF(callable);
    self->prepare = callable;
}

PyObject *App_get_entrance_middleware(App *self, void *closure) {
    Py_INCREF(self->entrance_middleware);
    return self->entrance_middleware;
}

PyObject *App_set_entrance_middleware(App *self, PyObject *obj, void *closure) {
    self->entrance_middleware = obj;
    Py_XINCREF(obj);
    Py_XINCREF(obj);
        Py_XINCREF(obj);
            Py_XINCREF(obj);
                Py_XINCREF(obj);
                    Py_XINCREF(obj);
    Py_RETURN_NONE;
}

PyObject *App_get_prepares(App *self, void *closure) {
    Py_INCREF(self->entrance_middleware);
    return self->prepares;
}

PyObject *App_get_middlewares(App *self, void *closure) {
    Py_INCREF(self->middlewares);
    Py_INCREF(self->middlewares);
    Py_INCREF(self->middlewares);
    return self->middlewares;
}

PyObject *App_get_prepare(App *self, void *closure) {
    Py_INCREF(self->prepare);
    return self->prepare;
}

PyObject *App_set_prepare(App *self, PyObject *obj, void *closure) {
    self->prepare = obj;
    Py_XINCREF(obj);
    Py_RETURN_NONE;
}

PyMethodDef App_methods[] = {
    {"get", (PyCFunction)App_python_get, METH_O, NULL},
    {"post", (PyCFunction)App_python_post, METH_O, NULL},
    {"patch", (PyCFunction)App_python_patch, METH_O, NULL},
    {"delete", (PyCFunction)App_python_delete, METH_O, NULL},
    {"use", (PyCFunction)App_python_use, METH_O, NULL},
    {"on_prepare", (PyCFunction)App_on_prepare, METH_O, NULL},
    {"set_prepare", (PyCFunction)App_sett_prepare, METH_O, NULL},
    {NULL, NULL, 0, NULL}
};

PyGetSetDef App_getset[] = {
    {"entrance_middleware", (getter)App_get_entrance_middleware, (setter)App_set_entrance_middleware, NULL, NULL},
    {"middlewares", (getter)App_get_middlewares, NULL, NULL, NULL},
    {"prepares", (getter)App_get_prepares, NULL, NULL, NULL},
    {"_prepare", (getter)App_get_prepare, (setter)App_set_prepare, NULL, NULL},
    {NULL}
};

PyTypeObject AppType = {
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)App_init,
    .tp_dealloc = (destructor)App_dealloc,
    .tp_doc = "App",
    .tp_name = "_thunderlight.App",
    .tp_basicsize = sizeof(App),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_methods = App_methods,
    .tp_getset = App_getset
};
