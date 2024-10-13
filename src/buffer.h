// Implements a buffer datastructure useful for reading/writing to a 
// dynamically sized buffer.

#include <stdlib.h>

struct buffer;

// Allocates a new buffer
struct buffer* buffer_create(size_t initial_size);

// Frees memory associated with given buffer and buffer itself.
void buffer_destroy(struct buffer* self);

// Ensures there is enough capacity to add `len` number of bytes.
void buffer_ensure_capacity(struct buffer* self, size_t len);

// Appends `len` bytes of the raw data pointed to by `data` 
void buffer_append(struct buffer* self, void* data, size_t len);

// Appends the given null-terminated string to the end of the buffer.
void buffer_append_str(struct buffer* self, char* str);

// Returns the pointer at the given offset in the buffer.
// Ensures the offset is within the bounds of the buffer.
char* buffer_offset2ptr(struct buffer* self, size_t offset);

// Returns the pointer associated with this buffer
char* buffer_getptr(struct buffer* self);

// Returns the length of the content in the buffer
size_t buffer_length(struct buffer* self);
