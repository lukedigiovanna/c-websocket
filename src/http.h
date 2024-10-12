
#include "bufio.h"

enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_UNKNOWN,
};

struct http_transaction {
    enum http_method method;


};

// Parses the given buffer to populate the given http_transaction.
// Returns 0 on success or -1 otherwise.
int http_parse(struct bufio* buff, struct http_transaction* ta);
void http_handle_request(struct http_transaction* ta);
