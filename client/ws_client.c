// C implementation of the Python client

// These are functionally equivalent

#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char* argv[]) {
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_socket_fd, (struct sockaddr*) &server_address, sizeof(server_address));

    

    close(client_socket_fd);

    return 0;
}