#include "context.h"
#include "space.h"


void Context_init(Context *self, Request *request, Response *response, Duostate *duostate) {
    self->request = request;
    self->response = response;
    self->duostate = duostate;
}

char *Context_repr(Context *self, char *head, uint8_t indent) {
    char *request_repr = Request_repr(self->request, NULL, 1);
    char *response_repr = Response_repr(self->response, NULL, 1);
    char *state_repr = Duostate_repr(self->duostate, NULL, 1);
    char *buffer = malloc(1024);
    buffer[0] = '\0';
    if (head != NULL) {
        strcat(buffer, head);
        strcat(buffer, " ");
    }
    strcat(buffer, "{\n");
    // req
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'req': ");
    strcat(buffer, request_repr);
    strcat(buffer, ",\n");
    // res
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'res': ");
    strcat(buffer, response_repr);
    strcat(buffer, ",\n");
    // state
    add_space(buffer, (indent + 1) * 4);
    strcat(buffer, "'state': ");
    strcat(buffer, state_repr);
    strcat(buffer, "\n");
    // end
    add_space(buffer, indent * 4);
    strcat(buffer, "}");
    free(request_repr);
    free(response_repr);
    free(state_repr);
    return buffer;
}
