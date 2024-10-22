
struct ws_server;

// Creates a websocket server object which will open a server on the given port
// but does not start listening for connections.
struct ws_server* ws_server_create(short port);

// Starts running the server
// This operation is blocking on the thread that calls it, so the server's
// event handlers should be set up before calling this function.
void ws_server_run(struct ws_server* self);
