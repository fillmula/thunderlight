#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "request.h"
#include "response.h"
#include "duostate.h"


typedef struct {
    Request *request;
    Response *response;
    Duostate *duostate;
} Context;

void Context_init(Context *self, Request *request, Response *response, Duostate *duostate);

#ifdef __cplusplus
}
#endif
