#include "ft_ping.h"
#include <math.h>

float avg(t_summary *summary) {
    float sum = 0;
    for (uint32_t i = 0; i < summary->received; i++) {
        sum += summary->delays[i];
    }
    return sum / summary->received;
}

float stddev(t_summary *summary, float mean) {
    float sum = 0;
    for (uint32_t i = 0; i < summary->received; i++) {
        sum += pow(summary->delays[i] - mean, 2);
    }
    return sqrt(sum / summary->received);
}