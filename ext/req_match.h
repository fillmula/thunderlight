#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>
#include "mresult.h"

extern PyTypeObject ReqMatchType;

typedef struct {
    PyObject_HEAD
    MatchResult *mresult;
} ReqMatch;

ReqMatch *ReqMatch_new(MatchResult *mresult);


#ifdef __cplusplus
}
#endif
