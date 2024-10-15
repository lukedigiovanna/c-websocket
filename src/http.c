#ifndef HTTP_H
#define HTTP_H

#include "http.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

// Return the appropriate HTTP method based on the method string.
static enum http_method get_method(char* method_str) {
    if (!strcasecmp("GET", method_str)) {
        return HTTP_GET;
    }
    else if (!strcasecmp("POST", method_str)) {
        return HTTP_POST;
    }
    else {
        return HTTP_UNKNOWN;
    }
}

static bool http_process_header(struct http_transaction* ta, char* header) {
    // First extract header name
    char* save_ptr;
    char* header_name = strtok_r(header, ": ", &save_ptr);
    char* header_value = strtok_r(NULL, " ", &save_ptr);
    
    if (!strcasecmp("Sec-WebSocket-Key", header)) {
        return true;
    }
    else if (!strcasecmp("Sec-WebSocket-Version", header)) {
        return true;
    }

    fprintf(stderr, "http_process_header: unrecognized header: %s\n", header_name);
    return false;
}

bool http_parse(struct http_transaction* ta, struct bufio* buff) {
    size_t req_offset, req_length;    
    char* req, *save_ptr;
    req_length = bufio_readline(buff, &req_offset);
    req = bufio_offset2ptr(buff, req_offset);
    req[req_length - 2] = '\0';
    // expect header to match format: <METHOD> <PATH> <VERSION>
    char* method_str = strtok_r(req, " ", &save_ptr);
    if (method_str == NULL) {
        return false;
    }
    char* path_str = strtok_r(NULL, " ", &save_ptr);
    if (path_str == NULL) {
        return false;
    }
    char* version_str = strtok_r(NULL, " ", &save_ptr);

    // TODO: CHANGE THIS TO SEND AN APPRORIATE ERROR RESPONSE.
    if (strcasecmp("HTTP/1.1", version_str)) {
        fprintf(stderr, "http_parse: this HTTP implementation only support HTTP/1.1 not %s", version_str);
        return -1;
    }

    ta->method = get_method(method_str);

    // read headers
    size_t header_offset, header_len;
    char* header;
    for (;;) {
        header_len = bufio_readline(buff, &header_offset);
        if (header_len < 0) {
            return false;
        }
        header = bufio_offset2ptr(buff, header_offset);
        if (header_len == 2 && !strcmp(header, "\r\n")) {
            break;
        }
        header[header_len - 2] = '\0';
        http_process_header(ta, header);
    }

    printf("finished parsing request\n");

    return 0;
}

void http_handle_request(struct http_transaction* ta) {

}

#endif