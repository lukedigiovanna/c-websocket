#include <stdio.h>

#include "socket.h"
#include "bufio.h"
#include "http.h"
#include "base64.h"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <string.h>

#define PORT 8080
#define MAX_MESSAGE_SIZE 256

static void handle_client(int client_fd) {
    struct bufio* buffer = bufio_create(client_fd);
    
    // parse as http request
    struct http_transaction ta;
    http_init_transaction(&ta, buffer);
    int rc;
    if ((rc = http_parse(&ta)) != 0) {
        fprintf(stderr, "http_parse: return non-zero rc=%d\n", rc);
        return;
    }

    struct buffer* response_buffer = buffer_create(1024);
    buffer_append_str(response_buffer, "HTTP/1.1 ");
    buffer_append_str(response_buffer, "101 Switching Protocols\r\n");
    buffer_append_str(response_buffer, "Other headers here.\r\n");
    bufio_send_buffer(buffer, response_buffer);
    buffer_destroy(response_buffer);

    bufio_destroy(buffer);
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