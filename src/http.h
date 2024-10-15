
#include "bufio.h"

#include <stdbool.h>

enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN,
};

struct http_transaction {
    enum http_method method;

    size_t websocketKeyOffset;
    size_t websocketVersionOffset;
};

// Parses the given buffer to populate the given http_transaction.
// Returns true on success
bool http_parse(struct http_transaction* ta, struct bufio* buff);

void http_handle_request(struct http_transaction* ta);
