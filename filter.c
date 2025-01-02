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

float calculate_alpha(float cutoff_frequency, float sampling_frequency) {
    return cutoff_frequency / (cutoff_frequency + sampling_frequency);
}