#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "websocket.h"

#include "base64.h"
#include "sha1.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define GUID_LENGTH 36
static const char guid[GUID_LENGTH + 1] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

int websocket_sec_handshake(char* out, const char* secret) {
    char buffer[64];
    char hashed[64];
    size_t secret_len = strlen(secret);
    size_t total_len = secret_len + GUID_LENGTH;
    // concatenate secret and guid
    memcpy(buffer, secret, secret_len);
    memcpy(buffer + secret_len, guid, GUID_LENGTH + 1);
    SHA1(hashed, buffer, total_len);
    // Length of a SHA-1 hash is always 20 bytes
    base64_encode(out, hashed, 20);
    return 0;
}

#endif