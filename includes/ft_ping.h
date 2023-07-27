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
#include <netdb.h>
#include "icmp.h"
#include "options.h"

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

#ifndef LOGGING_FD
    #define LOGGING_FD STDOUT_FILENO // stdout is used by inetutils-2.0's ping
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
bool get_ip(char *arg, struct sockaddr_in *output);

// Logging functions
void preping_info(char *host, char *ip, uint16_t size);
void postping_info(t_summary *summary);
void ping_info(t_summary *summary, t_icmp_packet *response_packet, uint8_t ttl, float response_time, struct sockaddr_in *sa);

// Wrappers
ssize_t send_packet(t_icmp_packet *packet, int sockfd, struct sockaddr_in *dest);
ssize_t receive_packet(int sockfd, uint8_t *buffer, size_t size);