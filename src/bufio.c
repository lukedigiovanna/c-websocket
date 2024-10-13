#ifndef BUFIO_H
#define BUFIO_H

#include "bufio.h"

#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

// Amount to read on each call to "recv"
#define READSIZE 2048

struct bufio {
    struct buffer* buffer; // Internal buffer used to store read content
    size_t head;  // Position in reading/writing
    int fd; // File descriptor to read from
};

struct bufio* bufio_create(int fd) {
    struct bufio* self = (struct bufio*) malloc(sizeof(struct bufio));

    if (self == NULL) {
        perror("failed to allocate bufio");
        exit(EXIT_FAILURE);
    }

    self->buffer = buffer_create(READSIZE);
    self->head = 0;
    self->fd = fd;

    return self;
}

void bufio_destroy(struct bufio* self) {
    buffer_destroy(self->buffer);
    free(self);
}

int bufio_readbyte(struct bufio* self, char* ch) {
    static char read_buffer[READSIZE];
    // Read more from socket if we already read everything in the buffer
    if (self->head >= buffer_length(self->buffer)) {
        ssize_t bytes = recv(self->fd, read_buffer, READSIZE, MSG_NOSIGNAL);
        
        if (bytes <= 0) {
            return -1;
        }

        buffer_append(self->buffer, read_buffer, bytes);
    }

    *ch = *bufio_offset2ptr(self, self->head);

    self->head++;

    return 0;
}

size_t bufio_readline(struct bufio* self) {
    size_t offset = self->head;

    char c;
    while (bufio_readbyte(self, &c) == 0 && c != '\n');

    size_t len = self->head - offset;
    *bufio_offset2ptr(self, len - 1) = '\0';

    return offset;
}

size_t bufio_read(struct bufio* self, size_t* len) {
    // size_t offset = self->head;
    // char c;
    // while (bufio_readbyte(self, &c) == 0);

    // if (len != NULL) {
    //     *len = self->head - offset;
    // }

    // return offset;
    return 0;
}

void bufio_appends(struct bufio* self, char* content) {

}

void bufio_write(struct bufio* self, int fd) {

}

char *bufio_offset2ptr(struct bufio* self, size_t offset) {
    return buffer_offset2ptr(self->buffer, offset);
}

#endif