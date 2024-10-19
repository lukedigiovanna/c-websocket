// Implements base64 encoding and decoding

#include <stddef.h>

// Returns the expected encode length of the given data length
size_t base64_encode_length(size_t data_len);

// Returns the expected decode length of a given base64 encoded string.
size_t base64_decode_length(const char* b_str);

// Encodes the given data as base64 into `out`.
void base64_encode(char* out, const void* data, size_t data_len);

// Decodes the given base64 encoded string `b_str`. Allocates memory at the
// pointer given by data and populates it with the decoded data from the string.
// Outputs the length of the decoded string in the `len` paramter if it is non-null.
// Returns a non-zero error code on failure.
int base64_decode(const char* b_str, void* out, size_t* len);
