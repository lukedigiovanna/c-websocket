#include <stdio.h>

#include "socket.h"
#include "bufio.h"
#include "http.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 256

static void handle_client(int client_fd) {
    struct bufio* buffer = bufio_create(client_fd);
    
    // parse as http request
    struct http_transaction ta;
    http_init_transaction(&ta, buffer);
    int rc;
    if ((rc = http_parse(&ta)) != 0) {
        fprintf(stderr, "http_parse: return non-zeor rc=%d\n", rc);
        return;
    }

    bufio_destroy(buffer);
}

int main(int argc, char* argv[]) {
    // Set up a socket server
    int server_socket_fd = server_bind_and_listen(PORT);

    printf("Listening to clients on 127.0.0.1:%d\n", PORT);

    for (;;) {
        // Wait for a connection
        struct sockaddr client_address;
        socklen_t len;
        int client_fd = accept(server_socket_fd, &client_address, &len);

        handle_client(client_fd);
    }

    close(server_socket_fd);

    return 0;
}