#include <stdio.h>

#include "socket.h"
#include "bufio.h"
#include "http.h"
#include "base64.h"
#include "websocket.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <string.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 256

static void handle_client(int client_fd) {
    struct http_transaction ta;
    http_init_transaction(&ta, client_fd);
    http_parse(&ta);
    http_handle_request(&ta);
    http_destroy_transaction(&ta);
}

int main(int argc, char* argv[]) {
    // Set up a socket server
    int server_socket_fd = server_bind_and_listen(PORT);

    printf("Listening to clients on 127.0.0.1:%d\n", PORT);

    for (;;) {
        // Wait for a connection
        struct sockaddr_in client_address;
        socklen_t len;
        int client_fd = accept(server_socket_fd, (struct sockaddr*) &client_address, &len);

        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), buf, INET_ADDRSTRLEN);

        fprintf(stderr, "Received client: %s:%d\n", buf, ntohs(client_address.sin_port));

        handle_client(client_fd);

        close(client_fd);
    }

    close(server_socket_fd);

    return 0;
}