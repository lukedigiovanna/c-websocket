#ifndef BASE64_H
#define BASE64_H

#include "base64.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static char b64_alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* base64_encode(void* data, size_t data_len) {
    // Determine padding and encoded length
    int r = (data_len * 4) % 3;
    // Padding: r=0 -> 0, r=1 -> 2, r=2 -> 1 
    size_t padding = r == 0 ? 0 : r == 1 ? 2 : 1;
    size_t characters = data_len * 4 / 3;
    size_t encoded_len = characters + padding;
    char* encoded = malloc(encoded_len + 1);
    encoded[encoded_len] = '\0'; // null terminate
    // c index in encoded string
    size_t bit_offset = 0;
    size_t data_bit_len = data_len * 8;
    if (data_len % 4 != 0) data_bit_len+=8;
    int c;
    size_t byte_offset;
    uint8_t value;
    for (c = 0; c < characters && bit_offset + 6 <= data_bit_len; c++) {
        byte_offset = bit_offset / 8;
        uint8_t current_byte = *((uint8_t*) (data + byte_offset));
        uint8_t next_byte = byte_offset + 1 >= data_len ? 0 : *((uint8_t*) (data + byte_offset + 1));
        if (bit_offset % 8 == 0) {
            // First 6 bits in current byte
            value = current_byte >> 2; // Take only first 6 bits
        }
        else if (bit_offset % 8 == 6) {
            // Last 2 bits in current byte + first 4 bits in next byte
            value = ((current_byte & 0b11) << 4) | (next_byte >> 4);
        }
        else if (bit_offset % 8 == 4) {
            // Last 4 bits in current byte + first 2 bits in next byte
            value = ((current_byte & 0b1111) << 2) | (next_byte >> 6);
        }
        else if (bit_offset % 8 == 2) {
            // Last 6 bits in current byte
            value = current_byte & 0b111111;
        }
        encoded[c] = b64_alphabet[value];
        bit_offset += 6;
    }
    for (int i = 0; i < padding; i++) {
        encoded[c++] = '=';
    }

    return encoded;
}

#endif