#ifndef SOCKET_H
#define SOCKET_H

#include "socket.h"

#include <sys/socket.h>
#include <netinet/in.h>

int server_bind_and_listen(short port) {
    // Set up a socket server
    int server_socket_fd;
    struct sockaddr_in server_address;

    // Create socket file descriptor
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
        return server_socket_fd;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket file descriptor to the given address
    int rc;
    if ((rc = bind(server_socket_fd, 
                  (struct sockaddr*) &server_address, 
                  sizeof(struct sockaddr_in))) != 0) {
        return rc;
    }

    // Start listening with a backlog of up to 5 pending clients
    if ((rc = listen(server_socket_fd, 5)) != 0) {
        return rc;
    }

    return server_socket_fd;
}

#endif
