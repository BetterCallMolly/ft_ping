#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <signal.h>
#include "icmp.h"

#ifndef DEFAULT_DATA_SIZE
    #define DEFAULT_DATA_SIZE 56
#endif

#ifndef DEFAULT_MIN_DELAY
    #define DEFAULT_MIN_DELAY 1000000
#endif

#ifndef MAX_ARG_SIZE
    #define MAX_ARG_SIZE 2000000 // 2MB
#endif

#ifndef MAX_SAMPLE_SIZE
    #define MAX_SAMPLE_SIZE 1000 // over is just stupid
#endif

#ifndef DEFAULT_TIMEOUT
    #define DEFAULT_TIMEOUT 3 // 3 seconds
#endif

typedef struct s_summary {
    char HOST_NAME[MAX_ARG_SIZE];
    uint32_t sent;
    uint32_t received;
    uint32_t lost;
    float min_delay;
    float max_delay;
    float delays[MAX_SAMPLE_SIZE];
} t_summary;

float avg(t_summary *summary);
float stddev(t_summary *summary, float mean);