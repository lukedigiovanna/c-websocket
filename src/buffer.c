#ifndef BUFFER_H
#define BUFFER_H

#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct buffer {
    char* content;
    size_t capacity;
    size_t length;
};

struct buffer* buffer_create(size_t initial_size) {
    struct buffer* self = malloc(sizeof(struct buffer)); 
    self->content = malloc(initial_size);
    self->length = 0;
    self->capacity = initial_size;
    return self;
}

void buffer_destroy(struct buffer* self) {
    free(self->content);
    free(self);
}

void buffer_ensure_capacity(struct buffer* self, size_t len) {
    if (self->length + len > self->capacity) {
        self->capacity = self->capacity * 2 + len;
        self->content = realloc(self->content, self->capacity);
        if (self->content == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }
}

void buffer_append(struct buffer* self, void* data, size_t len) {
    buffer_ensure_capacity(self, len);
    memcpy(self->content + self->length, data, len);
    self->length += len;
}

void buffer_append_str(struct buffer* self, char* str) {
    size_t len = strlen(str) + 1;
    buffer_append(self, str, len);
}

char* buffer_offset2ptr(struct buffer* self, size_t offset) {
    if (offset >= self->length) {
        fprintf(stderr, "buffer_offset2ptr: offset out of buffer bounds: %ld in buffer of size %ld\n", offset, self->length);
        exit(EXIT_FAILURE);
    }

    return self->content + offset;
}

size_t buffer_ptr2offset(struct buffer* self, char* ptr) {
    if (ptr < self->content || ptr >= self->content + self->length) {
        fprintf(stderr, "buffer_ptr2offset: ptr not within buffer");
        exit(EXIT_FAILURE);
    }

    return (size_t) (ptr - self->content);
}

char* buffer_getptr(struct buffer* self) {
    return self->content;
}

size_t buffer_length(struct buffer* self) {
    return self->length;
}

#endif