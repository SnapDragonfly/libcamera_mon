#ifndef DATA_FILTER_H
#define DATA_FILTER_H

float low_pass_filter(float input, float *previous_output, float alpha);
float calculate_alpha(float cutoff_frequency, float sampling_frequency);

#endif // DATA_FILTER_H