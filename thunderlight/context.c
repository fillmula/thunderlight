#include "context.h"


void Context_init(Context *self, Request *request, Response *response, Duostate *duostate) {
    self->request = request;
    self->response = response;
    self->duostate = duostate;
}
