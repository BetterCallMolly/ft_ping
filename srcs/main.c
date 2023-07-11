#include "ft_ping.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "usage: ft_ping [-hv] destination\n");
        fprintf(stderr, "       -h? help\n");
        exit(1);
    }
    if (strncmp(argv[1], "-h", 2) == 0 || strncmp(argv[1], "-?", 2) == 0) {
        fprintf(stderr, "usage: ft_ping [-hv] destination\n");
        fprintf(stderr, "       -h? help\n");
        exit(0);
    }

    // Parse IP (we'll only support IPv4 for now)
    struct sockaddr_in sa;
    int ret = inet_pton(AF_INET, argv[1], &(sa.sin_addr));
    if (ret == -1) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: %s: %s\n", argv[1], error);
        exit(1);
    } else if (ret == 0) {
        fprintf(stderr, "ft_ping: %s: Invalid IP address\n", argv[1]);
        exit(1);
    }

    // At this point, we have a valid IP address
    t_icmp_packet echo_request = {0};
    echo_request.header.type = ECHO_REQUEST;
    echo_request.header.code = 0;

    // Bytes 5-6 = identifier (random / can be set by user as long as it fits in 2 bytes)
    // Bytes 7-8 = sequence number (incremented by 1 for each packet sent)

    // Push our identifier in the packet
    uint16_t identifier = get_echo_identifier();
    echo_request.header.rest[0] = (identifier >> 8) & 0xFF;
    echo_request.header.rest[1] = identifier & 0xFF;

    // Push our sequence number in the packet
    uint16_t sequence_number = 0;
    echo_request.header.rest[2] = (sequence_number >> 8) & 0xFF;
    echo_request.header.rest[3] = sequence_number & 0xFF;

    compute_icmp_checksum(&echo_request);

    disasm_icmp_packet(&echo_request, false);
}
