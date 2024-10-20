
#include "bufio.h"

#include "buffer.h"

#include <stdbool.h>

enum http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_UNKNOWN,
};

enum http_version {
    HTTP_VERSION_1_1,
    HTTP_VERSION_UNKNOWN,
};

enum http_response_code {
    HTTP_SWITCHING_PROTOCOLS=101,
    HTTP_OK=200,
    HTTP_BAD_REQUEST=400,
};

struct http_transaction {
    enum http_method method;
    enum http_version version;
    size_t path_offset;

    bool websocket_upgrade;
    size_t websocket_key_offset;
    size_t websocket_version_offset;

    struct bufio* buffer;

    bool parse_error;

    enum http_response_code response_code;
    struct buffer* response;
};

// Gives default values to fields of given transaction object and sets
// the working buffer to read/send from in the transaction.
void http_init_transaction(struct http_transaction* ta, int client_fd);

// Frees the memory associated with the given transaction
// Does not free the transaction itself
void http_destroy_transaction(struct http_transaction* ta);

// Parses the given buffer to populate the given http_transaction.
// Returns true on success
void http_parse(struct http_transaction* ta);

// Starts building the response in the internal buffer managed by the given
// transaction. Should not be called more than once per transaction and
// the response code should be set before calling this.
void http_start_response_headers(struct http_transaction* ta);
void http_end_response_headers(struct http_transaction* ta);

// Adds the given header/value to the response.
void http_add_response_header(struct http_transaction* ta, const char* header, const char* value);

void http_add_response_content(struct http_transaction* ta, const char* content);

void http_send_response(struct http_transaction* ta);

// Processes the given parsed HTTP request.
// Returns true if the corresponding socket should remain open (in the case of
// establishing a websocket connection for example) or false otherwise.
bool http_handle_request(struct http_transaction* ta);
