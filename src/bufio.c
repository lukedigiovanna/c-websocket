#ifndef BUFIO_H
#define BUFIO_H

#include "bufio.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

// Amount to read on each call to "read"
#define READSIZE 2048

struct bufio {
    char* buffer; // Internal buffer
    size_t head;   // Position in reading/writing
    size_t length; // Amount of content read into buffer
    size_t capacity; // Available capacity of the buffer
    int fd; // File descriptor to read from
};

struct bufio* bufio_create(int fd) {
    struct bufio* self = (struct bufio*) malloc(sizeof(struct bufio));

    if (self == NULL) {
        perror("failed to allocate bufio");
        exit(EXIT_FAILURE);
    }

    self->buffer = malloc(READSIZE);
    self->head = 0;
    self->length = 0;
    self->capacity = READSIZE;
    self->fd = fd;

    return self;
}

void bufio_destroy(struct bufio* self) {
    free(self->buffer);
    free(self);
}

int bufio_readbyte(struct bufio* self, char* ch) {
    // Ensure capacity of buffer
    if (self->head + READSIZE >= self->capacity) {
        // Then need to read more into the buffer (double it)
        size_t new_size = self->capacity * 2 + READSIZE;
        self->buffer = realloc(self->buffer, new_size);

        if (self->buffer == 0) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Read more from socket if we already read everything in the buffer
    if (self->head >= self->length) {
        ssize_t bytes = recv(self->fd, self->buffer + self->head, READSIZE, MSG_NOSIGNAL);
        
        if (bytes < 0) {
            perror("recv");
            return -1;
        }

        self->length += bytes;

        self->buffer[self->length] = '\0';
        printf("READ (%d bytes):\n%s\n", self->length, self->buffer);
    }

    *ch = self->buffer[self->head++];

    printf("Read byte: %d\n", *ch);

    return 0;
}

size_t bufio_readline(struct bufio* self, size_t* len) {
    size_t offset = self->head;

    char c;
    while (bufio_readbyte(self, &c) == 0 && c != '\n');

    *len = self->head - offset;

    return offset;
}

size_t bufio_read(struct bufio* self, size_t* len) {
    
    return NULL;
}

void bufio_appends(struct bufio* self, char* content) {

}

void bufio_write(struct bufio* self, int fd) {

}

char *bufio_offset2ptr(struct bufio* self, size_t offset) {
    return self->buffer + offset;
}

#endif