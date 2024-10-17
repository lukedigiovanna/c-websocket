// Implements base64 encoding and decoding

#include <stddef.h>

// Encodes the given string as base64
// Returns a new allocated string with the encoding
// (memory should be appropriately managed)
char* base64_encode(void* data, size_t data_len);

// Decodes the given string as base64
// Returns a new allocated data with the decoding
// (memory should be appropriately managed)
void* base64_decode(char* b_str, size_t* len);
