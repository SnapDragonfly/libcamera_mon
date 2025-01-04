#ifndef DATA_FILTER_H
#define DATA_FILTER_H

#include <time.h>

float low_pass_filter(float input, float *previous_output, float alpha);
float low_pass_filter_time(float input, float *previous_output, float alpha, float delta_time);
float calculate_alpha(float cutoff_frequency, float sampling_frequency);
double calculate_delta_time(struct timespec *start, struct timespec *end);

#endif // DATA_FILTER_H