# Python implementation of the C client

# These are functionally equivalent

import socket
import base64
import hashlib
import struct

HOST = '127.0.0.1'  # Assume local server.
PORT = 8080
RESOURCE = '/'

# WebSocket handshake function
def websocket_handshake(sock):
    # Generate WebSocket handshake headers
    key = base64.b64encode(b"test_key").decode('utf-8')
    headers = (
        f"GET {RESOURCE} HTTP/1.1\r\n"
        f"Host: {HOST}:{PORT}\r\n"
        f"Upgrade: websocket\r\n"
        f"Connection: Upgrade\r\n"
        f"Sec-WebSocket-Key: {key}\r\n"
        f"Sec-WebSocket-Version: 13\r\n\r\n"
    )

    # Send handshake request
    sock.send(headers.encode('utf-8'))

    # Receive server response
    response = sock.recv(1024).decode('utf-8')
    if "101 Switching Protocols" not in response:
        raise Exception("Handshake failed!")
    print("Handshake successful!\n", response)


# Encode the WebSocket frame for sending a message
def encode_frame(data):
    frame_head = bytearray()
    frame_head.append(0x81)  # First byte: FIN bit set, text frame (opcode 0x1)
    
    # Encode length
    length = len(data)
    if length <= 125:
        frame_head.append(length)
    elif length >= 126 and length <= 65535:
        frame_head.append(126)
        frame_head.extend(struct.pack(">H", length))
    else:
        frame_head.append(127)
        frame_head.extend(struct.pack(">Q", length))
    
    return frame_head + data.encode('utf-8')


# Decode the WebSocket frame received
def decode_frame(sock):
    frame = sock.recv(2)
    length = frame[1] & 127  # Second byte has the length

    if length == 126:
        length = struct.unpack(">H", sock.recv(2))[0]
    elif length == 127:
        length = struct.unpack(">Q", sock.recv(8))[0]

    masks = sock.recv(4)  # Mask key
    encoded = sock.recv(length)

    # Decode the data
    decoded = bytearray([encoded[i] ^ masks[i % 4] for i in range(len(encoded))])
    return decoded.decode('utf-8')


# Main WebSocket client function
def websocket_client():
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))

    try:
        # Perform WebSocket handshake
        websocket_handshake(sock)

        # Send a message (frame)
        message = "Hello WebSocket!"
        print(f"Sending: {message}")
        frame = encode_frame(message)
        sock.send(frame)

        # Receive a message (frame)
        received_message = decode_frame(sock)
        print(f"Received: {received_message}")

    finally:
        sock.close()


if __name__ == "__main__":
    websocket_client()
