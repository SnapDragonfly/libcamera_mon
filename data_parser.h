#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include "ring_buffer.h"

// Define enum for sampling methods
typedef enum {
    ARITHMETIC_FILTER = 0, // Arithmetic + filter
    RAW_FILTER = 1,        // Raw + filter
    ARITHMETIC = 2,        // Arithmetic only
    RAW = 3                // Raw only
} SamplingMethod;

int parse_buf_data_raw(RingBuffer *ring_buffer, int *frame, float *fps, int *size, size_t *parsed_bytes);
int parse_buf_data_arithmetic(RingBuffer *ring_buffer, int *frame, float *fps, int *size, size_t *parsed_bytes);

#endif // DATA_PARSER_H
