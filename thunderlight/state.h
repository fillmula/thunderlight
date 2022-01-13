#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "duostate.h"


typedef struct {
    Duostate *duostate;
} State;

static PyTypeObject StateType;

PyObject *State_new(Duostate *duostate);

#ifdef __cplusplus
}
#endif
