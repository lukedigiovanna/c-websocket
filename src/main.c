#include <stdio.h>

#include "socket.h"
#include "bufio.h"
#include "http.h"
#include "base64.h"
#include "websocket.h"

#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <string.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 256

static bool http_handle_client(int client_fd) {
    struct http_transaction ta;
    http_init_transaction(&ta, client_fd);
    http_parse(&ta);
    bool rc = http_handle_request(&ta);
    http_destroy_transaction(&ta);
    return rc;
}

int main(int argc, char* argv[]) {
    // Set up a socket server
    int server_socket_fd = server_bind_and_listen(PORT);

    printf("Listening to clients on 127.0.0.1:%d\n", PORT);

    struct pollfd fds[1024]; // this should be dynamic at some point
    memset(fds, 0, 1024 * sizeof(struct pollfd));
    nfds_t numfds = 1;
    fds[0].fd = server_socket_fd;
    fds[0].events = POLLIN;

    for (;;) {
        int ready = poll(fds, numfds, -1);

        if (ready == -1) {
            perror("poll");
            break;
        }

        for (nfds_t i = 0; i < numfds; i++) {
            if (fds[i].events != 0) {
                if (fds[i].fd == server_socket_fd) {
                    // Wait for a connection
                    struct sockaddr_in client_address;
                    socklen_t len;
                    int client_fd = accept(server_socket_fd, (struct sockaddr*) &client_address, &len);

                    char buf[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(client_address.sin_addr), buf, INET_ADDRSTRLEN);

                    fprintf(stderr, "Received client: %s:%d\n", buf, ntohs(client_address.sin_port));

                    bool keep_alive = http_handle_client(client_fd);

                    if (!keep_alive) {
                        close(client_fd);
                    }
                    else {
                        // Add the client to the list of file descriptors to watch
                        fds[numfds].fd = client_fd;
                        fds[numfds].events = POLLIN;
                        numfds++;
                    }
                }
                else { // Assume from client file descriptor
                    char buff[1024];
                    int bread = recv(fds[i].fd, buff, 1024, 0);
                    if (bread < 0) {
                        perror("recv");
                    }
                    buff[bread] = '\0';
                    printf("received: %s\n", buff);
                }
            }
        }
    }

    close(server_socket_fd);

    return 0;
}