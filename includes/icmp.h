#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define ICMP4_FRAME_SIZE 20
#define ICMP4_HEADER_SIZE 8
#define MAX_RECV_SIZE 0xFFFF + ICMP4_FRAME_SIZE
#define EMPTY_PACKET_SIZE 1 + sizeof(uint8_t) * 2 + sizeof(uint16_t) * 3

/*
Most of the informations about ICMP are from:
    - https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
    - https://tools.ietf.org/html/rfc792
    - https://tools.ietf.org/html/rfc1071
*/

// https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Control_messages
typedef enum e_icmp_types {
    ECHO_REPLY = 0,
    DESTINATION_UNREACHABLE = 3,
    SOURCE_QUENCH = 4,
    REDIRECT = 5,
    ECHO_REQUEST = 8,
    ROUTER_ADVERTISEMENT = 9,
    ROUTER_SOLICITATION = 10,
    TIME_EXCEEDED = 11,
    PARAMETER_PROBLEM = 12,
    TIMESTAMP = 13,
    TIMESTAMP_REPLY = 14,
    INFORMATION_REQUEST = 15,
    INFORMATION_REPLY = 16,
    ADDRESS_MASK_REQUEST = 17,
    ADDRESS_MASK_REPLY = 18,
    TRACEROUTE = 30,
    DATAGRAM_CONVERSION_ERROR = 31,
    MOBILE_HOST_REDIRECT = 32,
    IPV6_WHERE_ARE_YOU = 33,
    IPV6_I_AM_HERE = 34,
    MOBILE_REGISTRATION_REQUEST = 35,
    MOBILE_REGISTRATION_REPLY = 36,
    DOMAIN_NAME_REQUEST = 37,
    DOMAIN_NAME_REPLY = 38,
    SKIP = 39,
    PHOTURIS = 40,
    EXPERIMENTAL_MOBILITY_PROTOCOL = 41,
    EXTENDED_ECHO_REQUEST = 42,
    EXTENDED_ECHO_REPLY = 43,
} t_icmp_types;

// https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol#Header

typedef struct s_icmp_packet {
    t_icmp_types type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
    uint8_t data[MAX_RECV_SIZE];
    uint32_t size;
} t_icmp_packet;

bool compute_icmp_checksum(t_icmp_packet *packet);
uint8_t *serialize_icmp_packet(t_icmp_packet *packet);
t_icmp_packet *deserialize_icmp_packet(char *packet);
int32_t checksum(uint8_t *buffer, uint32_t n);

// Utils functions
uint16_t get_echo_identifier(void);
uint32_t get_timestamp(void);
void generate_data(uint8_t *buffer, uint32_t size);

// Debug functions
void disasm_icmp_packet(t_icmp_packet *packet, bool disasm_data);