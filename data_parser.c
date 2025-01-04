
#include <string.h>
#include <stdio.h>

#include "data_parser.h"
#include "config.h"

int parse_buf_data_raw(RingBuffer *ring_buffer, int *frame, float *fps, int *size, size_t *parsed_bytes) {
    // Ensure the ring buffer has sufficient data to parse
    if (ring_buffer->data_size < 16) {
        return -1;  // Not enough data to parse
    }

    // Copy data from the ring buffer into a temporary buffer for easier parsing
    char temp_buffer[ring_buffer->data_size + 1]; // +1 for null terminator
    size_t i;
    for (i = 0; i < ring_buffer->data_size; i++) {
        temp_buffer[i] = ring_buffer->data[(ring_buffer->start + i) % ring_buffer->size];
    }
    temp_buffer[ring_buffer->data_size] = '\0'; // Null-terminate the string

    // Variables to track parsing results
    int frame_found = 0;
    int fps_found = 0;
    int size_found = 0;

    // Tokenize the input buffer by newline
    char *token = strtok(temp_buffer, "\n");

    while (token) {
        // Debug
        //printf("token: %s\n", token);

        //safety measurement 1
        extern int required_data_size;
        if (token - temp_buffer > required_data_size*0.8) {
            printf("safety measurement 1: %ld:%s\n", token - temp_buffer, token);
            break;
        }

        //safety measurement 2
        if (strlen(token) > 55) {
            printf("safety measurement 2: %s\n", token);
            return -5;
        }
        
        // Parse the frame number from lines starting with "Viewfinder frame"
        if (strncmp(token, "Viewfinder frame", 16) == 0) {
            if (sscanf(token, "Viewfinder frame %d", frame) == 1) {
                frame_found = 1;
            }
        }

        // Parse FPS from lines starting with "#"
        if (strncmp(token, "#", 1) == 0) {
            if (sscanf(token, "#%*d (%f fps)", fps) == 1) {
                fps_found = 1;
            }
        }

        // Parse size from lines containing "FileOutput: output buffer"
        if (strstr(token, "FileOutput: output buffer") != NULL) {
            if (sscanf(token, "FileOutput: output buffer %*s size %d", size) == 1) {
                size_found = 1;
            }
        }

        // Move to the next line
        token = strtok(NULL, "\n");
    }

    // Check if all necessary components were successfully parsed
    if (frame_found && fps_found && size_found) {
        *parsed_bytes = ring_buffer->data_size;  // Assume all bytes were parsed
        return 0;  // Success
    }

    // Return appropriate error code based on what failed
    if (!frame_found) return -2;  // Failed to parse frame
    if (!fps_found) return -3;    // Failed to parse FPS
    if (!size_found) return -4;   // Failed to parse size

    return -10;  // Unknown error
}

int parse_buf_data_arithmetic(RingBuffer *ring_buffer, int *frame, float *fps, int *size, size_t *parsed_bytes) {
    // Ensure token operation will NOT overflow
    char temp_buffer[RING_BUFFER_SIZE+1]; // +1 for null terminator

    if (ring_buffer->data_size < 16) {
        return -1;  // Not enough data to parse
    }

    size_t buffer_limit = sizeof(temp_buffer) - 1;
    size_t bytes_to_copy = ring_buffer->data_size > buffer_limit ? buffer_limit : ring_buffer->data_size;

    size_t i;
    for (i = 0; i < bytes_to_copy; i++) {
        temp_buffer[i] = ring_buffer->data[(ring_buffer->start + i) % ring_buffer->size];
    }
    temp_buffer[ring_buffer->data_size] = '\0'; // Null-terminate the string

    int frame_found = 0, fps_found = 0, size_found = 0;
    int temp_frame = 0, temp_size = 0, data_count = 0;
    float temp_fps = 0, total_fps = 0;
    int total_size = 0;
    size_t current_offset = 0;

    char *token = strtok(temp_buffer, "\n");

    while (token) {
        size_t token_length = strlen(token);

        // Debugging to verify offsets
        //printf("token: %s\n", token);
        //printf("Current offset: %ld, token: %s\n", current_offset, token);

        // Safety measurement 1
        extern int required_data_size;
        if (current_offset > required_data_size * 0.8) {
            printf("SM 1: %ld:%s\n", current_offset, token);
            break;
        }

        // Safety measurement 2
        if (token_length > 55) {
            printf("SM 2: %s\n", token);
            return -5;
        }

        // Parse the frame number from lines starting with "Viewfinder frame"
        if (strncmp(token, "Viewfinder frame", 16) == 0) {
            if (sscanf(token, "Viewfinder frame %d", &temp_frame) == 1) {
                frame_found = 1;
                *frame = temp_frame;
            }
        }

        // Parse FPS from lines starting with "#"
        if (frame_found && strncmp(token, "#", 1) == 0) {
            int frame_number = 0;
            if (sscanf(token, "#%d (%f fps)", &frame_number, &temp_fps) == 2) {
                if (frame_number <= temp_frame) {
                    fps_found = 1;
                } else {
                    frame_found = fps_found = size_found = 0;
                    token = strtok(NULL, "\n");
                    continue;
                }
            }
        }

        // Parse size from lines containing "FileOutput: output buffer"
        if (frame_found && strstr(token, "FileOutput: output buffer") != NULL) {
            if (sscanf(token, "FileOutput: output buffer %*s size %d", &temp_size) == 1) {
                size_found = 1;
            } else {
                frame_found = fps_found = size_found = 0;
                token = strtok(NULL, "\n");
                continue;
            }
        }

        current_offset += token_length + 1; // Account for '\n'

        if (frame_found && fps_found && size_found) {
            total_fps += temp_fps;
            total_size += temp_size;
            data_count++;

            *parsed_bytes = current_offset;

            // DEBUG
            //printf("Count: %d Frame: %d Offset: %ld\n", data_count, temp_frame, *parsed_bytes);

            frame_found = fps_found = size_found = 0;
        }

        token = strtok(NULL, "\n");
    }

    // Multi-frame detected
    if (data_count > 1) {
        *frame = -data_count;
    }

    // Compute the average FPS if at least one valid FPS was found
    if (data_count > 0) {
        *fps = total_fps / data_count;
        *size = total_size / data_count;
        return 0;  // Success
    }

    // Return appropriate error code based on what failed
    if (!frame_found) return -2;  // Failed to parse frame
    if (!fps_found) return -3;    // Failed to parse FPS
    if (!size_found) return -4;   // Failed to parse size

    return -10;  // Unknown error
}


