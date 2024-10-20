#ifndef HTTP_H
#define HTTP_H

#include "http.h"
#include "websocket.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>

#define CRLF "\r\n"

// Return the appropriate HTTP method based on the method string.
static enum http_method get_method(char* method_str) {
    if (!strcasecmp("GET", method_str)) {
        return HTTP_METHOD_GET;
    }
    else if (!strcasecmp("POST", method_str)) {
        return HTTP_METHOD_POST;
    }
    else {
        return HTTP_METHOD_UNKNOWN;
    }
}

static enum http_version get_version(char* version_str) {
    if (!strcasecmp("HTTP/1.1", version_str)) {
        return HTTP_VERSION_1_1;
    }
    else {
        return HTTP_VERSION_UNKNOWN;
    }
}

static const char* get_response_code_str(enum http_response_code rc) {
    switch (rc) {
    case HTTP_BAD_REQUEST:
        return "400 Bad Request";
    case HTTP_SWITCHING_PROTOCOLS:
        return "101 Switching Protocols";
    case HTTP_OK:
        return "200 OK";
    default:
        return "UNKNOWN";
    }
}

static const char* get_version_str(enum http_version version) {
    switch (version) {
    case HTTP_VERSION_1_1:
        return "HTTP/1.1";
    default:
        return "HTTP/1.1";
    }
}

void http_init_transaction(struct http_transaction* ta, int client_fd) {
    ta->buffer = bufio_create(client_fd);
    ta->websocket_key_offset = 0;
    ta->websocket_version_offset = 0;
    ta->websocket_upgrade = false;
    ta->method = HTTP_METHOD_UNKNOWN;
    ta->version = HTTP_VERSION_UNKNOWN;
    ta->response = buffer_create(256);
}

void http_destroy_transaction(struct http_transaction* ta) {
    buffer_destroy(ta->response);
    bufio_destroy(ta->buffer);
}

static bool http_process_header(struct http_transaction* ta, char* header) {
    // First extract header name
    char* save_ptr;
    char* header_name = strtok_r(header, ": ", &save_ptr);
    if (header_name == NULL) {
        return false;
    }
    char* header_value = strtok_r(NULL, " ", &save_ptr);
    if (header_value == NULL) {
        return false;
    }
    if (!strcasecmp("Sec-WebSocket-Key", header_name)) {
        ta->websocket_key_offset = bufio_ptr2offset(ta->buffer, header_value);
    }
    else if (!strcasecmp("Sec-WebSocket-Version", header_name)) {
        ta->websocket_version_offset = bufio_ptr2offset(ta->buffer, header_value);
    }
    else if (!strcasecmp("Upgrade", header_name)) {
        ta->websocket_upgrade = true;
    }
    return true;
}

void http_parse(struct http_transaction* ta) {
    size_t req_offset; 
    ssize_t req_length;    
    char* req, *save_ptr;
    req_length = bufio_readline(ta->buffer, &req_offset);
    if (req_length <= 0) {
        ta->parse_error = true;
        return;
    }
    req = bufio_offset2ptr(ta->buffer, req_offset);
    req[req_length - 2] = '\0';
    // expect header to match format: <METHOD> <PATH> <VERSION>
    char* method_str = strtok_r(req, " ", &save_ptr);
    if (method_str == NULL) {
        ta->parse_error = true;
        return;
    }
    char* path_str = strtok_r(NULL, " ", &save_ptr);
    if (path_str == NULL) {
        ta->parse_error = true;
        return;
    }
    char* version_str = strtok_r(NULL, " ", &save_ptr);
    if (version_str == NULL) {
        ta->parse_error = true;
        return;
    }

    ta->version = get_version(version_str);
    ta->method = get_method(method_str);
    ta->path_offset = bufio_ptr2offset(ta->buffer, path_str);

    // read headers
    size_t header_offset;
    ssize_t header_len;
    char* header;
    for (;;) {
        header_len = bufio_readline(ta->buffer, &header_offset);
        if (header_len < 0) {
            ta->parse_error = true;
            return;
        }
        header = bufio_offset2ptr(ta->buffer, header_offset);
        if (header_len == 2) { // If just 2 then assume only read the CRLF
            break;
        }
        header[header_len - 2] = '\0'; // Remove the CRLF from the string
        if (!http_process_header(ta, header)) {
            ta->parse_error = true;
            return;
        }
    }

    ta->parse_error = false;
}

void http_start_response_headers(struct http_transaction* ta) {
    buffer_append_str(ta->response, get_version_str(ta->version));
    buffer_append_str(ta->response, " ");
    buffer_append_str(ta->response, get_response_code_str(ta->response_code));
    buffer_append_str(ta->response, CRLF);
}

void http_add_response_header(struct http_transaction* ta, const char* header, const char* value) {
    buffer_append_str(ta->response, header);
    buffer_append_str(ta->response, ": ");
    buffer_append_str(ta->response, value);
    buffer_append_str(ta->response, CRLF);
}

void http_end_response_headers(struct http_transaction* ta) {
    buffer_append_str(ta->response, CRLF);
}

void http_add_response_content(struct http_transaction* ta, const char* content) {
    buffer_append_str(ta->response, content);
}

void http_send_response(struct http_transaction* ta) {
    printf("Sending response:\n%s\n", buffer_getptr(ta->response));
    bufio_send_buffer(ta->buffer, ta->response);
}

static void http_send_bad_request(struct http_transaction* ta, const char* reason) {
    char buff[256];
    sprintf(buff, "{\"message\": \"%s\"}", reason);
    ta->response_code = HTTP_BAD_REQUEST;
    http_start_response_headers(ta);
    char snum[6];
    sprintf(snum, "%lu", strlen(buff));
    http_add_response_header(ta, "Content-Length", snum);
    http_add_response_header(ta, "Content-Type", "application/json");
    http_end_response_headers(ta);
    http_add_response_content(ta, buff);
    http_send_response(ta);
}

bool http_handle_request(struct http_transaction* ta) {
    if (ta->parse_error) {
        http_send_bad_request(ta, "Malformed HTTP request");
        return false;
    }
    if (ta->version != HTTP_VERSION_1_1) {
        http_send_bad_request(ta, "Only HTTP/1.1 is supported");
        return false;
    }
    if (ta->websocket_upgrade) {
        if (ta->websocket_key_offset) {
            char sec_buffer[64];
            char* websocket_key = bufio_offset2ptr(ta->buffer, ta->websocket_key_offset);
            websocket_compute_secret(sec_buffer, websocket_key);
            printf("got ws key: %s -- compute secret: %s\n", websocket_key, sec_buffer);
            ta->response_code = HTTP_SWITCHING_PROTOCOLS;
            http_start_response_headers(ta);
            http_add_response_header(ta, "Sec-WebSocket-Accept", sec_buffer);
            http_add_response_header(ta, "Connection", "Upgrade");
            http_add_response_header(ta, "Upgrade", "websocket");
            http_end_response_headers(ta);
            http_send_response(ta);
            return true;
        }
    }
    http_send_bad_request(ta, "Unsupported operation");
    return false;
}

#endif