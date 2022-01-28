#include <sys/param.h>
#include <stdlib.h>
#include "matcher.h"
#include "not_found.h"


typedef struct {
    char *pos;
    size_t len;
} Segment;

MatcherList *MatcherList_alloc(void) {
    MatcherList *self = malloc(sizeof(MatcherList));
    return self;
}

void MatcherList_init(MatcherList *self, const char *name) {
    self->name = name;
    self->capacity = MATCHER_LIST_INITIAL_SIZE;
    self->length = 0;
    self->buffer = self->inline_buffer;
}

MatcherList *MatcherList_new(const char *name) {
    MatcherList *self = MatcherList_alloc();
    MatcherList_init(self, name);
    return self;
}

void MatcherList_dealloc(MatcherList *self) {
    for (size_t i = 0; i < self->length; i++) {
        Py_DECREF(self->buffer[i].handler);
    }
    if (self->buffer != self->inline_buffer) {
        free(self->buffer);
    }
    free(self);
}

void Matcher_record(Matcher *self) {
    char *pos = (char *)self->route;
    self->is_static = true;
    self->ele_num = 0;
    size_t this_ele_len = 0;
    size_t this_ele_idx = 0;
    bool next_ele = false;
    self->statics[0] = true;
    self->ele_lens[0] = 0;
    while (*pos != '\0') {
        if (*pos == '/') {
            if (pos != self->route) {
                // save current
                char *key = malloc(this_ele_len);
                memcpy(key, pos - this_ele_len, this_ele_len);
                self->keys[this_ele_idx] = key;
                // next
                self->ele_lens[this_ele_idx] = this_ele_len;
                this_ele_idx += 1;
                self->statics[this_ele_idx] = true;
            }
            next_ele = true;
            this_ele_len = -1;
        } else {
            if (*pos == ':') {
                self->is_static = false;
                self->statics[this_ele_idx] = false;
                this_ele_len--;
            }
            if (next_ele) {
                self->ele_num++;
                next_ele = false;
            }
        }
        this_ele_len++;
        pos++;
    }
    if (pos != self->route) {
        // save current
        char *key = malloc(this_ele_len);
        memcpy(key, pos - this_ele_len, this_ele_len);
        self->keys[this_ele_idx] = key;
        // next
        self->ele_lens[this_ele_idx] = this_ele_len;
        this_ele_idx += 1;
        self->statics[this_ele_idx] = true;
    }
    self->len = pos - self->route;
    // // debug print
    // printf("recorded '%s', elements %u, static %s\n", self->route, self->ele_num, self->is_static ? "true" : "false");
    // if (!self->is_static) {
    //     for (size_t i = 0; i < self->ele_num; i++) {
    //         printf("segment '%s', len %u, static %s\n", self->keys[i], self->ele_lens[i], self->statics[i] ? "true" : "false");
    //     }
    // }
}

void MatcherList_append(MatcherList *self, const char *route, PyObject *handler) {
    size_t index = self->length;
    if (index > self->capacity) { // TODO: unlikely
        self->capacity += 32;
        if (self->buffer == self->inline_buffer) {
            self->buffer = malloc(self->capacity * sizeof(Matcher));
            memcpy(self->buffer, self->inline_buffer, MATCHER_LIST_INITIAL_SIZE * sizeof(Matcher));
        } else {
            self->buffer = realloc(self->buffer, self->capacity * sizeof(Matcher));
        }
    }
    Py_INCREF(handler);
    Py_INCREF(handler);
    self->buffer[index].handler = handler;
    self->buffer[index].route = route;
    Matcher_record(&self->buffer[index]);
    self->length++;
}

bool Matcher_match(Matcher *self, char *path, Request *request) {
    size_t len = strlen(path);
    if (*(path + len - 1) == '/') {
        if (len > 1) {
            len--;
            path[len] = '\0';
        }
    }
    if (self->is_static) {
        if (len != self->len) {
            return false;
        }
        if (strcmp(path, self->route) == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        Segment segments[MATCHER_MAX_ITEMS];
        size_t segments_len = 0;
        size_t seg_len = 0;
        for (size_t i = 0; i < len; i++) {
            char *pos = path + i;
            if (*pos == '/') {
                if (segments_len > 0) {
                    segments[segments_len - 1].len = seg_len;
                    segments[segments_len - 1].pos = pos - seg_len;
                }
                segments_len++;
                seg_len = 0;
            } else if (i == len - 1) {
                if (segments_len > 0) {
                    segments[segments_len - 1].pos = pos - seg_len;
                    seg_len++;
                    segments[segments_len - 1].len = seg_len;
                }
            } else {
                seg_len++;
            }
        }
        // printf("segments length is %zu\n", segments_len);
        // for (size_t i = 0; i < segments_len; i++) {
        //     printf("segment at index %zu, length %zu, content %s\n", i, segments[i].len, segments[i].pos);
        // }
        if (segments_len != (size_t)self->ele_num) {
            return false;
        }
        size_t mresult_len = 0;
        for (size_t i = 0; i < self->ele_num; i++) {
            if (self->statics[i]) {
                if (self->ele_lens[i] != segments[i].len) {
                    return false;
                }
                if (strncmp(segments[i].pos, self->keys[i], MAX(segments[i].len, self->ele_lens[i])) != 0) {
                    return false;
                }
            } else {
                mresult_len++;
                // record
            }
        }
        size_t mresult_idx = 0;
        MatchResult *mresult = MatchResult_new(mresult_len);
        for (uint8_t i = 0; i < self->ele_num; i++) {
            if (!self->statics[i]) {
                char *value = malloc(segments[i].len);
                strncpy(value, segments[i].pos, segments[i].len);
                MatchResult_set(mresult, mresult_idx, self->keys[i], value);
                mresult_idx++;
            }
        }
        request->mresult = mresult;
        return true;
    }
}

PyObject *MatcherList_match(MatcherList *self, char *path, Request *request) {
    for (size_t i = 0; i < self->length; i++) {
        if (Matcher_match(&self->buffer[i], path, request)) {
            Py_INCREF(self->buffer[i].handler);
            return self->buffer[i].handler;
        }
    }
    Py_INCREF(not_found);
    return not_found;
}
