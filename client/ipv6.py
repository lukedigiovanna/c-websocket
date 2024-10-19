import socket

def ipv6_client():
    # Define the server address and port
    server_address = ("::1", 8080)  # "::1" is the IPv6 loopback address, port 8080

    # Create an IPv6 socket
    client_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

    try:
        # Connect to the server
        print(f"Connecting to {server_address[0]} on port {server_address[1]}...")
        client_socket.connect(server_address)

        # Send an HTTP GET request
        http_request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        client_socket.sendall(http_request.encode())

        # Receive and print the response
        response = client_socket.recv(4096)
        print("Received response:")
        print(response.decode())

    except socket.error as e:
        print(f"Socket error: {e}")
    
    finally:
        # Close the socket connection
        client_socket.close()

if __name__ == "__main__":
    ipv6_client()
