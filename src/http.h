
#include "bufio.h"

#include <stdbool.h>

enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN,
};

struct http_transaction {
    enum http_method method;
    size_t path_offset;

    bool websocket_upgrade;
    size_t websocket_key_offset;
    size_t websocket_version_offset;

    struct bufio* buffer;
};

// Gives default values to fields of given transaction object and sets
// the working buffer to read/send from in the transaction.
void http_init_transaction(struct http_transaction* ta, struct bufio* buff);

// Parses the given buffer to populate the given http_transaction.
// Returns true on success
bool http_parse(struct http_transaction* ta);

void http_handle_request(struct http_transaction* ta);
