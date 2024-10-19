// Provides functions for establishing a websocket connection
// includes performing handshake logic
// 

// Represents a websocket connection, used to keep track of a connection
// and send a receive socket events from.
struct websocket {
    int sockfd;
};

// Initializes the given websocket struct
int websocket_init(struct websocket* ws, int sockfd);

// Computes the handshake secret used to establish the websocket connection
// Populates `out` with the computed handshake accept value using the given
// base64 encoded `secret`. Returns non-zero error code on failure.
int websocket_sec_handshake(char* out, const char* secret);
