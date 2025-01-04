#include "filter.h"
#include <stdio.h>
#include <math.h>

// Low-pass filter function with alpha as a parameter
float low_pass_filter(float input, float *previous_output, float alpha) {
    // Apply the low-pass filter formula
    float output = alpha * input + (1 - alpha) * (*previous_output);

    // Update previous output
    *previous_output = output;

    return output;
}

float low_pass_filter_time(float input, float *previous_output, float alpha, float delta_time) {
    float effective_alpha = delta_time / (delta_time + alpha);
    float output = effective_alpha * input + (1 - effective_alpha) * (*previous_output);
    *previous_output = output;
    return output;
}

float calculate_alpha(float cutoff_frequency, float sampling_frequency) {
    return cutoff_frequency / (cutoff_frequency + sampling_frequency);
}
double calculate_delta_time(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}
