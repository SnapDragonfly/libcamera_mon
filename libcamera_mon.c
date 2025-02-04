#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "data_parser.h"
#include "ring_buffer.h"
#include "filter.h"
#include "config.h"
#include "log.h"
#include "version.h"

// Global variables to hold configurable parameters
int buffer_size                     = FIFO_BUFFER_SIZE;
int required_data_size              = REQUIRED_DATA_SIZE;

char fifo_path[DEFAULT_PATH_LENGTH] = FIFO_PATH;
char proc_path[DEFAULT_PATH_LENGTH] = PROC_PATH;

SamplingMethod sampling_method      = SAMPLING_METHOD;
float sampling_frequency            = SAMPLING_FREQUENCY;
float cutoff_frequency              = CUTOFF_FREQUENCY;

extern LogLevel global_log_level;

LogLevel parse_log_level(const char *level_str) {
    if (strcmp(level_str, "DEBUG") == 0) return LOG_DEBUG;
    if (strcmp(level_str, "INFO") == 0) return LOG_INFO;
    if (strcmp(level_str, "WARNING") == 0) return LOG_WARNING;
    if (strcmp(level_str, "ERROR") == 0) return LOG_ERROR;
    if (strcmp(level_str, "FATAL") == 0) return LOG_FATAL;
    fprintf(stderr, "Unknown log level: %s\n", level_str);
    exit(EXIT_FAILURE);
}

void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("  Ver: %s Compiled at: %s\n", GIT_VERSION, VERSION_STRING);
    printf("Options:\n");
    printf("  --buffer-size <value>        Set the buffer size (default: %d)\n", FIFO_BUFFER_SIZE);
    printf("  --required-data-size <value> Set the required data size (default: %d)\n", REQUIRED_DATA_SIZE);
    printf("  --fifo-path <path>           Set the FIFO file path (default: %s)\n", FIFO_PATH);
    printf("  --sampling-method <value>    Set the sampling method (default: %d)\n", SAMPLING_METHOD);
    printf("                               0: arithmetic+filter\n");
    printf("                               1: raw+filter\n");
    printf("                               2: arithmetic\n");
    printf("                               3: raw\n");
    printf("  --sample-frequency <value>   Set the sampling frequency in Hz (default: %d)\n", SAMPLING_FREQUENCY);
    printf("  --cutoff-frequency <value>   Set the cutoff frequency in Hz (default: %d)\n", CUTOFF_FREQUENCY);
    printf("  --log-level <LEVEL>          Set log level, default: %s\n", "INFO");
    printf("                               DEBUG, INFO, WARNING, ERROR, FATAL\n");
    printf("  --help                       Show this help message\n");
}

int parse_arguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--buffer-size") == 0 && i + 1 < argc) {
            buffer_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--required-data-size") == 0 && i + 1 < argc) {
            required_data_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--fifo-path") == 0 && i + 1 < argc) {
            strncpy(fifo_path, argv[++i], sizeof(fifo_path) - 1);
            fifo_path[sizeof(fifo_path) - 1] = '\0'; // Ensure null-termination
        } else if (strcmp(argv[i], "--sampling_frequency") == 0 && i + 1 < argc) {
            sampling_frequency = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--cutoff-frequency") == 0 && i + 1 < argc) {
            cutoff_frequency = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--sampling-method") == 0 && i + 1 < argc) {
            sampling_method = atoi(argv[++i]);
            if (sampling_method < 0 || sampling_method > 3) {
                fprintf(stderr, "Invalid sampling method: %d. Valid values are 0, 1, 2, or 3.\n", sampling_method);
                return -1;
            }
        } else if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc) {
            global_log_level = parse_log_level(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return -1;
        }
    }
    return 0;
}

int ensure_path_exists(const char *path, int is_fifo, mode_t create_mode) {
    // Check if the path exists
    if (access(path, F_OK) == -1) {
        if (is_fifo) {
            // Create FIFO file
            if (mkfifo(path, create_mode) == -1) {
                LOG_ERROR("Error creating FIFO file");
                return -1;
            }
            LOG_INFO("FIFO file '%s' created successfully.\n", path);
        } else {
            // Create directory or regular file
            if (mkdir(path, create_mode) == -1 && errno != EEXIST) {
                LOG_ERROR("Error creating directory");
                return -1;
            }
            LOG_INFO("Directory '%s' created successfully.\n", path);
        }
    } else {
        LOG_INFO("Path '%s' already exists.\n", path);
    }
    return 0;
}

int open_fifo(const char *fifo_path) {
    int fifo_fd = open(fifo_path, O_RDONLY);
    if (fifo_fd == -1) {
        LOG_ERROR("Error opening FIFO file");
        return -1;
    }
    LOG_INFO("FIFO file '%s' opened successfully. Waiting for input...\n", fifo_path);
    return fifo_fd;
}

int open_proc(const char *proc_path) {
    int proc_fd = open(proc_path, O_WRONLY);
    if (proc_fd == -1) {
        LOG_ERROR("Error opening PROC file");
        return -1;
    }
    LOG_INFO("PROC file '%s' opened successfully. Waiting for write...\n", proc_path);
    return proc_fd;
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    if (parse_arguments(argc, argv) != 0) {
        return -1;
    }

    // Print the configuration
    LOG_INFO("Configuration:\n");
    LOG_INFO("  Buffer size: %d\n", buffer_size);
    LOG_INFO("  Required data size: %d\n", required_data_size);
    LOG_INFO("  FIFO path: %s\n", fifo_path);
    LOG_INFO("  Sampling frequency: %.2f Hz\n", sampling_frequency);
    LOG_INFO("  Cutoff frequency: %.2f Hz\n", cutoff_frequency);
    LOG_INFO("  Sampling method: %d\n", sampling_method);
    LOG_INFO("    (0: arithmetic+filter, 1: raw+filter, 2: arithmetic, 3: raw)\n");
    LOG_INFO("Current log level: %d\n", global_log_level);

    float previous_output    = 0.0f;  // Initial previous output value
    
    // Calculate alpha based on cutoff frequency and sampling frequency
    float alpha = calculate_alpha(cutoff_frequency, sampling_frequency);
    LOG_INFO("Calculated alpha: %.4f\n", alpha);

    if (ensure_path_exists(DIR_PATH, 0, 0777) != 0) {
        return EXIT_FAILURE;
    }

    // Ensure the FIFO file exists
    if (ensure_path_exists(fifo_path, 1, 0666) != 0) {
        return EXIT_FAILURE;
    }

    // Open the FIFO file
    int fifo_fd = open_fifo(fifo_path);
    if (fifo_fd == -1) {
        return EXIT_FAILURE;
    }

    // Open the PROC file
    int proc_fd = open_proc(proc_path);
    if (proc_fd == -1) {
        return EXIT_FAILURE;
    }

    // Buffer for reading data from FIFO
    char buffer[buffer_size];
    ssize_t bytes_read;

    // Ring buffer setup
    RingBuffer ring_buffer;
    init_ring_buffer(&ring_buffer, RING_BUFFER_SIZE);

    // Loop to read data from the FIFO
    while (1) {
        bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            // Store the data in the ring buffer
            write_ring_buffer(&ring_buffer, buffer, bytes_read);

            // Attempt to parse data only when there's sufficient data in the ring buffer
            while (ring_buffer.data_size >= required_data_size) {
                // Try to parse the data from the ring buffer
                int frame;
                float fps;
                float bitrateRM, bitrateFM;
                int size;
                size_t parsed_bytes;

                int parse_result = -1;
                switch(sampling_method) {
                    case ARITHMETIC_FILTER:
                    case ARITHMETIC:
                        parse_result = parse_buf_data_arithmetic(&ring_buffer, &frame, &fps, &size, &parsed_bytes);
                        break;
                    case RAW_FILTER:
                    case RAW:
                        parse_result = parse_buf_data_raw(&ring_buffer, &frame, &fps, &size, &parsed_bytes);
                        break;
                    default:
                        print_usage(argv[0]);
                        return -1;
                        break;
                }

                if (parse_result == 0) {
                    // Successfully parsed data

                    float filtered_size = 0;
                    switch(sampling_method) {
                        case ARITHMETIC_FILTER:
                        case RAW_FILTER:
                            static struct timespec previous_time = {0, 0};
                            struct timespec current_time;
                            double delta_time;

                            clock_gettime(CLOCK_MONOTONIC, &current_time);
                            if (previous_time.tv_sec == 0 && previous_time.tv_nsec == 0) {
                                // default 0 for the first time
                                delta_time = 0.0;
                            } else {
                                delta_time = calculate_delta_time(&previous_time, &current_time);
                            }
                            previous_time = current_time;

                            filtered_size = low_pass_filter_time(size, &previous_output, alpha, delta_time);
                            break;
                        case ARITHMETIC:
                        case RAW:
                        default:
                            break;
                    }

                    bitrateRM = size * 8 * fps / 1000000;
                    bitrateFM = filtered_size * 8 * fps / 1000000;

                    char proc_buffer[PROC_BUFFER_SIZE];

                    snprintf(proc_buffer, PROC_BUFFER_SIZE,
                            "frame=%d, fps=%.2f, size=%d, bitrate=%.2f Mbps, fsize=%d, fbitrate=%.2f Mbps\n", 
                                        frame, fps, size, bitrateRM, (int)filtered_size, bitrateFM);

                    ssize_t bytes_written = write(proc_fd, proc_buffer, strlen(proc_buffer));
                    if (bytes_written < 0) {
                        LOG_ERROR("Failed to write to /proc/libcamera_proc");
                    } else {
                        LOG_INFO("%ld: %s", bytes_written, proc_buffer);
                    }

                    // Discard the parsed data from the ring buffer
                    remove_ring_buffer_data(&ring_buffer, parsed_bytes);
                } else {
                    // Parsing failed, discard all data in the ring buffer
                    LOG_WARNING("Failed(%d) to parse data. Discarding all data in the ring buffer.\n", parse_result);
                    clear_ring_buffer(&ring_buffer);
                    break;  // Exit the loop to reattempt with new data
                }
            }

            // Provide feedback to user
            if (is_print_log_level(LOG_DEBUG)) {
                printf(".");
                fflush(stdout);  // Force the buffer to flush
            }
        } else if (bytes_read == 0) {
            // End-of-file (FIFO was closed on the writer's end)
            LOG_INFO("No more data. Waiting for libcamera log message...\n");
            sleep(1); // Avoid busy-waiting
        } else {
            LOG_FATAL("Error reading from FIFO");
            break;
        }
    }

    // Close the FIFO file
    close(fifo_fd);
    close(proc_fd);
    LOG_INFO("FIFO file '%s' closed.\n", fifo_path);
    LOG_INFO("PROC file '%s' closed.\n", proc_path);

    // Clean up ring buffer
    cleanup_ring_buffer(&ring_buffer);

    return EXIT_SUCCESS;
}
