#ifndef BASE64_H
#define BASE64_H

#include "base64.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static char b64_alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Do b64invs[char - 43] to get value corresponding to character
static int b64invs[80] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
                        59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
                        6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                        21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
                        29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                        43, 44, 45, 46, 47, 48, 49, 50, 51 };

static inline int min(int a, int b) { return a < b ? a : b; }

char* base64_encode(void* data, size_t data_len) {
    // Every 3 bytes of data is 4 base64 characters
    size_t num_blocks = (data_len - 1) / 3 + 1;
    size_t encoded_chars_len = 2 + ((data_len - 1) / 3) * 4 + (data_len + 2) % 3;
    size_t encoded_len = num_blocks * 4;
    char* encoded = malloc(encoded_len + 1);
    encoded[encoded_len] = '\0';
    int c = 0;
    uint8_t* b_data = (uint8_t*) data;
    for (int i = 0; i < num_blocks; i++) {
        // Encode the 3-byte block
        for (;c<min(i*4+4,encoded_chars_len);c++) {
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

static inline bool is_valid_b64_char(char c) {
    return ('A' <= c && c <= 'Z') 
        || ('a' <= c && c <= 'z')
        || ('0' <= c && c <= '9')
        || c == '+' || c == '/';
}

int base64_decode(char* b_str, void** data, size_t* data_len) {
    size_t len = strlen(b_str);
    // count the ='s at the end.
    size_t padding = 0;
    for (int i = len - 1; i >= 0 && b_str[i] == '='; i--) {
        padding++;
    }
    // maximum padding should be 2
    if (padding > 2) {
        return -1;
    }
    size_t encoded_chars_len = len - padding;
    size_t decoded_len = 1 + (encoded_chars_len - 1) / 4 * 3 + (encoded_chars_len + 2) % 4;
    uint8_t* decoded = malloc(decoded_len + 1);
    decoded[decoded_len] = 0; // Always put a null terminator after to simplify possible conversions to textual data.
    int c = 0;
    for (int i = 0; i < encoded_chars_len; i+=4, c+=3) {
        if (!is_valid_b64_char(b_str[i])) return -1;
        size_t decoded_block = 0;
        int j = 0;
        for (j = 0; j < 4; j++) {
            if (i + j < encoded_chars_len) {
                if (!is_valid_b64_char(b_str[i + j])) return -2;
                int val = b64invs[b_str[i + j] - 43];
                decoded_block |= val << 6 * (3 - j);
            }
        }
        decoded[c] = (decoded_block >> 16) & 0xFF;
        if (j >= 3) {
            decoded[c + 1] = (decoded_block >> 8) & 0xFF;
        }
        if (j >= 4) {
            decoded[c + 2] = decoded_block & 0xFF;
        }
    }
    *data = (void*) decoded;
    return 0;
}

#endif