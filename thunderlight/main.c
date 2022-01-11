#include "main.h"

PyObject *gimme(void) {
    Py_RETURN_NONE;
}

static PyMethodDef MainMethods[] = {
    {"gimme", gimme, METH_NOARGS, "Get the global default app object."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyModuleDef Main_module = {
    PyModuleDef_HEAD_INIT,
    "main",
    "main",
    -1,
    MainMethods, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_main(void) {
    PyObject* m = NULL;
    m = PyModule_Create(&Main_module);
    if (!m) {
        goto error;
    }
    goto finally;
error:
    m = NULL;
finally:
    return m;
}
