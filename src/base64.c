#ifndef BASE64_H
#define BASE64_H

#include "base64.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static char b64_alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline int min(int a, int b) { return a < b ? a : b; }

char* base64_encode(void* data, size_t data_len) {
    // Every 3 bytes of data is 4 base64 characters
    size_t num_blocks = (data_len - 1) / 3 + 1;
    size_t encoded_chars = 2 + ((data_len - 1) / 3) * 4 + (data_len + 2) % 3;
    size_t encoded_len = num_blocks * 4;
    char* encoded = malloc(encoded_len + 1);
    encoded[encoded_len] = '\0';
    int c = 0;
    uint8_t* b_data = (uint8_t*) data;
    for (int i = 0; i < num_blocks; i++) {
        // Encode the 3-byte block
        for (;c<min(i*4+4,encoded_chars);c++) {
            size_t v;
            switch (c % 4) {
                case 0:
                    v = b_data[i*3] >> 2;
                    break;
                case 1:
                    v = ((b_data[i*3] & 0b11) << 4) | (b_data[i*3+1] >> 4);
                    break;
                case 2:
                    v = ((b_data[i*3+1] & 0b1111) << 2) | (b_data[i*3+2] >> 6);
                    break;
                case 3:
                    v = b_data[i*3+2] & 0b111111;
                    break;
            }
            encoded[c] = b64_alphabet[v];
        }
    }
    for (;c<encoded_len;c++) {
        encoded[c] = '=';
    }
    return encoded;
}

#endif