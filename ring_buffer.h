#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t size;
    size_t start;
    size_t end;
    size_t data_size;  // Current size of the data in the ring buffer
} RingBuffer;

// Function prototypes
void init_ring_buffer(RingBuffer *ring_buffer, size_t size);
void write_ring_buffer(RingBuffer *ring_buffer, const char *data, size_t size);
int read_ring_buffer(RingBuffer *ring_buffer, char *data, size_t size);
void remove_ring_buffer_data(RingBuffer *ring_buffer, size_t size);
void clear_ring_buffer(RingBuffer *ring_buffer);
void cleanup_ring_buffer(RingBuffer *ring_buffer);

#endif // RING_BUFFER_H
