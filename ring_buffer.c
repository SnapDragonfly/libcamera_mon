
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ring_buffer.h"
#include "log.h"

void init_ring_buffer(RingBuffer *ring_buffer, size_t size) {
    ring_buffer->data = (char *)malloc(size);
    if (ring_buffer->data == NULL) {
        perror("Failed to allocate memory for ring buffer");
        exit(EXIT_FAILURE);
    }
    ring_buffer->size = size;
    ring_buffer->start = 0;
    ring_buffer->end = 0;
    ring_buffer->data_size = 0;
}

void write_ring_buffer(RingBuffer *ring_buffer, const char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        ring_buffer->data[ring_buffer->end] = data[i];
        ring_buffer->end = (ring_buffer->end + 1) % ring_buffer->size;
        if (ring_buffer->data_size < ring_buffer->size) {
            ring_buffer->data_size++;
        } else {
            ring_buffer->start = (ring_buffer->start + 1) % ring_buffer->size; // Overwrite if buffer is full
        }
    }
}

int read_ring_buffer(RingBuffer *ring_buffer, char *data, size_t size) {
    if (ring_buffer->data_size < size) {
        return -1;  // Not enough data to read
    }
    for (size_t i = 0; i < size; i++) {
        data[i] = ring_buffer->data[ring_buffer->start];
        ring_buffer->start = (ring_buffer->start + 1) % ring_buffer->size;
    }
    ring_buffer->data_size -= size;
    return 0;
}

void remove_ring_buffer_data(RingBuffer *ring_buffer, size_t size) {
    LOG_DEBUG("ring before, start: %ld size: %ld remove: %ld\n", ring_buffer->start, ring_buffer->data_size, size);

    ring_buffer->start = (ring_buffer->start + size) % ring_buffer->size;
    ring_buffer->data_size -= size;

    LOG_DEBUG("ring remove, start: %ld size: %ld remove: %ld\n", ring_buffer->start, ring_buffer->data_size, size);
}

void clear_ring_buffer(RingBuffer *ring_buffer) {
    ring_buffer->start = 0;
    ring_buffer->end = 0;
    ring_buffer->data_size = 0;
}

void cleanup_ring_buffer(RingBuffer *ring_buffer) {
    free(ring_buffer->data);
}
