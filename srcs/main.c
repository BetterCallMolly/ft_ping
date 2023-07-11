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
    echo_request.type = ECHO_REQUEST;
    echo_request.code = 0;

    // Bytes 5-6 = identifier (random / can be set by user as long as it fits in 2 bytes)
    // Bytes 7-8 = sequence number (incremented by 1 for each packet sent)

    // Push our identifier in the packet
    uint16_t identifier = get_echo_identifier();
    echo_request.rest[0] = (identifier >> 8) & 0xFF;
    echo_request.rest[1] = identifier & 0xFF;

    // Generate a buffer of n bytes of data
    generate_data(echo_request.data, DEFAULT_DATA_SIZE);

    // Set data length
    echo_request.size = DEFAULT_DATA_SIZE;

    // Surround the below block with a loop to send multiple packets (for now, we'll only send one)
    {
        // Push our sequence number in the packet
        uint16_t sequence_number = 0;
        echo_request.rest[2] = (sequence_number >> 8) & 0xFF;
        echo_request.rest[3] = sequence_number & 0xFF;

        // Push our timestamp in the packet
        echo_request.timestamp = get_timestamp(); // TODO: check if we need to convert to network byte order
    }

    // Create a raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: socket: %s\n", error);
        exit(1);
    }

    // Set socket options
    int ttl = 64;

    // Set TTL
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: setsockopt: %s\n", error);
        exit(1);
    }

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr = sa.sin_addr;
    dest.sin_port = 0;

    compute_icmp_checksum(&echo_request);

    // Serialize packet
    uint8_t *raw_packet = serialize_icmp_packet(&echo_request);

    disasm_icmp_packet(&echo_request, true);

    for (size_t i = 0; i < sizeof(echo_request) - 4; i++) {
        printf("%02x ", raw_packet[i]);
    }

    // Send packet
    ssize_t bytes_sent = sendto(sockfd, raw_packet, sizeof(echo_request) - 4, 0, (struct sockaddr *)&dest, sizeof(dest));
    if (bytes_sent < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: sendto: %s\n", error);
        exit(1);
    }
    printf("Sent %ld bytes\n", bytes_sent);
}
