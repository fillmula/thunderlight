#include <Python.h>
#include "app.h"
#include "protocol.h"
#include "ctx.h"
#include "req_headers.h"
#include "req_match.h"
#include "req.h"
#include "res_headers.h"
#include "res.h"
#include "state.h"
#include "not_found.h"
#include "middleware_chain.h"
#include "server.h"
#include "json.h"
#include "applier.h"


PyObject *global_app = NULL;

PyObject *Thunderlight_main(PyObject *module, PyObject *app) {
    if (global_app != NULL) {
        Py_DECREF(global_app);
    }
    global_app = app;
    Py_INCREF(global_app);
    Py_INCREF(global_app);
    return global_app;
}

PyObject *Thunderlight_gimme(PyObject *module) {
    Py_INCREF(global_app);
    return global_app;
}

PyObject *Thunderlight_make(PyObject *module) {
    return Thunderlight_main(module, App_native_new());
}

PyObject *Thunderlight_use(PyObject *module, PyObject *arg) {
    Py_INCREF(global_app);
    App_use(global_app, arg);
    Py_RETURN_NONE;
}

PyObject *Thunderlight_get(PyObject *module, PyObject *arg) {
    return App_python_get(global_app, arg);
}

PyObject *Thunderlight_post(PyObject *module, PyObject *arg) {
    return App_python_post(global_app, arg);
}

PyObject *Thunderlight_patch(PyObject *module, PyObject *arg) {
    return App_python_patch(global_app, arg);
}

PyObject *Thunderlight_delete(PyObject *module, PyObject *arg) {
    return App_python_delete(global_app, arg);
}

PyObject *Thunderlight_apply(PyObject *module, PyObject *middleware) {
    return Applier_new(middleware);
}

void Thunderlight_final_setup(void) {
    global_app = App_native_new();
    Py_INCREF(global_app);
}

PyMethodDef module_methods[] = {
    {"main", (PyCFunction)Thunderlight_main, METH_O, NULL},
    {"gimme", (PyCFunction)Thunderlight_gimme, METH_NOARGS, NULL},
    {"make", (PyCFunction)Thunderlight_make, METH_NOARGS, NULL},
    {"use", (PyCFunction)Thunderlight_use, METH_O, NULL},
    {"get", (PyCFunction)Thunderlight_get, METH_O, NULL},
    {"post", (PyCFunction)Thunderlight_post, METH_O, NULL},
    {"patch", (PyCFunction)Thunderlight_patch, METH_O, NULL},
    {"delete", (PyCFunction)Thunderlight_delete, METH_O, NULL},
    {"apply", (PyCFunction)Thunderlight_apply, METH_O, NULL},
    {NULL}
};

PyModuleDef thunderlight = {
    PyModuleDef_HEAD_INIT,
    .m_name = "thunderlight",
    .m_doc = "Thunderlight is the fastest Python HTTP server.",
    .m_size = -1,
    .m_methods = module_methods
};

PyMODINIT_FUNC PyInit_thunderlight(void) {
    JSON_setup();
    StatusMessage_setup();
    PyObject *module = PyModule_Create(&thunderlight);
    PyType_Ready(&CtxType);
    PyModule_AddType(module, &CtxType);
    PyType_Ready(&ReqHeadersType);
    PyModule_AddType(module, &ReqHeadersType);
    PyType_Ready(&ReqMatchType);
    PyModule_AddType(module, &ReqMatchType);
    PyType_Ready(&ReqType);
    PyModule_AddType(module, &ReqType);
    PyType_Ready(&ResHeadersType);
    PyModule_AddType(module, &ResHeadersType);
    PyType_Ready(&ResType);
    PyModule_AddType(module, &ResType);
    PyType_Ready(&StateType);
    PyModule_AddType(module, &StateType);
    PyType_Ready(&NotFoundType);
    PyModule_AddType(module, &NotFoundType);
    PyType_Ready(&NotFoundIteratorType);
    PyModule_AddType(module, &NotFoundIteratorType);
    PyType_Ready(&ServerType);
    PyModule_AddType(module, &ServerType);
    PyType_Ready(&AppType);
    PyModule_AddType(module, &AppType);
    PyType_Ready(&RouteWrapperType);
    PyModule_AddType(module, &RouteWrapperType);
    PyType_Ready(&ProtocolType);
    PyModule_AddType(module, &ProtocolType);
    PyType_Ready(&ChainedMiddlewareType);
    PyModule_AddType(module, &ChainedMiddlewareType);
    PyType_Ready(&ChainedMiddlewareIteratorType);
    PyModule_AddType(module, &ChainedMiddlewareIteratorType);
    PyType_Ready(&OuterNextType);
    PyModule_AddType(module, &OuterNextType);
    PyType_Ready(&OuterNextIteratorType);
    PyModule_AddType(module, &OuterNextIteratorType);
    PyType_Ready(&ApplierType);
    PyModule_AddType(module, &ApplierType);
    PyType_Ready(&AppliedHandlerType);
    PyModule_AddType(module, &AppliedHandlerType);
    PyType_Ready(&AppliedHandlerIteratorType);
    PyModule_AddType(module, &AppliedHandlerIteratorType);
    not_found = PyType_GenericNew(&NotFoundType, NULL, NULL);
    Py_INCREF(not_found);
    PyModule_AddObject(module, "not_found", not_found);
    Py_INCREF(Py_None);
    PyModule_AddObject(module, "Next", Py_None);
    Py_INCREF(Py_None);
    PyModule_AddObject(module, "Handler", Py_None);
    Py_INCREF(Py_None);
    PyModule_AddObject(module, "Middleware", Py_None);
    Thunderlight_final_setup();
    return module;
}
