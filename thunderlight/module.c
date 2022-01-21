#include <Python.h>
#include "app.h"
#include "protocol.h"
#include "ctx.h"
#include "req_headers.h"
#include "req.h"
#include "res_headers.h"
#include "res.h"
#include "state.h"
#include "not_found.h"
#include "middleware_chain.h"
#include "server.h"
#include "json.h"


PyModuleDef thunderlight = {
    PyModuleDef_HEAD_INIT,
    .m_name = "thunderlight",
    .m_doc = "Thunderlight is the fastest Python HTTP server.",
    .m_size = -1
};

PyMODINIT_FUNC PyInit_thunderlight(void) {
    JSON_setup();
    StatusMessage_setup();
    PyObject *module = PyModule_Create(&thunderlight);
    PyType_Ready(&CtxType);
    PyModule_AddType(module, &CtxType);
    PyType_Ready(&ReqHeadersType);
    PyModule_AddType(module, &ReqHeadersType);
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
    not_found = PyType_GenericNew(&NotFoundType, NULL, NULL);
    Py_INCREF(not_found);
    PyModule_AddObject(module, "not_found", not_found);
    PyModule_AddObject(module, "Next", Py_None);
    return module;
}
