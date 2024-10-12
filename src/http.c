#ifndef HTTP_H
#define HTTP_H

#include "http.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

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

int http_parse(struct bufio* buff, struct http_transaction* ta) {
    char* header = bufio_offset2ptr(buff, bufio_readline(buff));
    // expect header to match format: <METHOD> <PATH> <VERSION>
    // Use thread-safe strtok: strtok_r
    char* save_ptr;
    char* method_str = strtok_r(header, " ", &save_ptr);
    char* path_str = strtok_r(NULL, " ", &save_ptr);
    char* version_str = strtok_r(NULL, " ", &save_ptr);
    printf("Method: %s\n", method_str);
    printf("Path: %s\n", path_str);
    printf("Version: %s\n", version_str);

    ta->method = get_method(method_str);

    return 0;
}

void http_handle_request(struct http_transaction* ta) {

}

#endif