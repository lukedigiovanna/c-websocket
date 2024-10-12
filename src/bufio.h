// A buffer is used for writing and reading to a socket

#include <stddef.h>

// Opaque type
struct bufio;

// Initializes a bufio
struct bufio* bufio_create(int fd);

// Frees the memory associated with this bufio and the bufio itself.
void bufio_destroy(struct bufio* self);

// Reads a single byte from the file descriptor and populates the `ch` out var.
// Most of the time this will return from the internal buffer, making this
// operation very cheap. When we reach the end of the buffer, the buffer
// will be extended and more data will be read from the file descriptor.
// Returns -1 on error, and 0 otherwise
int bufio_readbyte(struct bufio* self, char* ch);

// Reads the content of the buffer up until the next LF
// Returns the offset of the pointer in the buffer and outputs the length
// of the line into `len`.
size_t bufio_readline(struct bufio* self, size_t* len);

// Reads the remaining content of the buffer until EOF
// Stores the length of the read content into `len`
size_t bufio_read(struct bufio* self, size_t* len);

// Appends the given `content` to the internal buffer
void bufio_appends(struct bufio* self, char* content);

// Writes the contents of the buffer to the given file descriptor, `fd`
void bufio_write(struct bufio* self, int fd);

char* bufio_offset2ptr(struct bufio* self, size_t offset);
