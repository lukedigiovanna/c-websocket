// Implements base64 encoding and decoding

#include <stddef.h>

// Encodes the given string as base64
// Returns a new allocated string with the encoding
// (memory should be appropriately managed)
char* base64_encode(void* data, size_t data_len);

// Decodes the given base64 encoded string `b_str`. Allocates memory at the
// pointer given by data and populates it with the decoded data from the string.
// Outputs the length of the decoded string in the `len` paramter.
// Returns a non-zero error code on failure.
int base64_decode(char* b_str, void** data, size_t* len);
