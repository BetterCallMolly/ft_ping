#include "ft_ping.h"

static t_summary g_summary = {0};

void sigint_handler(int signum) {
    (void) signum;
    printf("--- %s ping statistics ---\n", g_summary.HOST_NAME);
    printf("%d packets transmitted, %d received, %.2f%% packet loss\n",
        g_summary.sent,
        g_summary.received,
        (float) (g_summary.lost / g_summary.sent * 100)
    );
    float avg_delay = avg(&g_summary);
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
        g_summary.min_delay,
        avg_delay,
        g_summary.max_delay,
        stddev(&g_summary, avg_delay)
    );
    exit(0);
}

void ping_timeout(int signum) {
    (void) signum;
    // dprintf(STDERR_FILENO, "No response from %s\n", g_summary.HOST_NAME);
    g_summary.lost++;
}

void init_summary(char *arg) {
    strncpy(g_summary.HOST_NAME, arg, MAX_ARG_SIZE);
    g_summary.min_delay = 2147483647;
    g_summary.max_delay = 0;
}

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
    echo_request.identifier = get_echo_identifier();

    // Generate a buffer of n bytes of data
    generate_data(echo_request.data, DEFAULT_DATA_SIZE);

    // Set data length
    echo_request.size = DEFAULT_DATA_SIZE;

    // Setup signal handlers
    init_summary(argv[1]);
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, ping_timeout);

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

    // Set destination
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr = sa.sin_addr;
    dest.sin_port = 0;

    const size_t send_size = EMPTY_PACKET_SIZE + echo_request.size;
    const size_t receive_size = ICMP4_FRAME_SIZE + EMPTY_PACKET_SIZE + echo_request.size;

    printf("PING %s (%s): %d data bytes.\n", argv[1], inet_ntoa(sa.sin_addr), DEFAULT_DATA_SIZE);
    // Surround the below block with a loop to send multiple packets (for now, we'll only send one);
    while (1) {
        // Push our timestamp in the packet
        echo_request.checksum = 0;
        compute_icmp_checksum(&echo_request);

        // Serialize packet
        uint8_t *raw_packet = serialize_icmp_packet(&echo_request);

        float before = get_timestamp();
        // Send packet
        ssize_t bytes_sent = sendto(
            sockfd,
            raw_packet,
            send_size,
            0,
            (struct sockaddr *)&dest, sizeof(dest)
        );
        if (bytes_sent < 0) {
            char *error = strerror(errno);
            fprintf(stderr, "ft_ping: sendto: %s\n", error);
            exit(1);
        }
        g_summary.sent++;

        alarm(DEFAULT_TIMEOUT);
        // Receive packet
        uint8_t buffer[IP_MAXPACKET];
        ssize_t bytes_received = recvfrom(sockfd, buffer, receive_size, 0, NULL, NULL);
        if (bytes_received < 0) {
            char *error = strerror(errno);
            fprintf(stderr, "ft_ping: recvmsg: %s\n", error);
            exit(1);
        }
        alarm(0);

        g_summary.received++;
        uint8_t version = buffer[0] >> 4;
        if (version != 4) {
            fprintf(stderr, "ft_ping: recvmsg: Invalid IP version\n");
            exit(1);
        }

        uint8_t header_length = buffer[0] & 0x0F;
        if (header_length < 5) {
            fprintf(stderr, "ft_ping: recvmsg: Invalid IP header length\n");
            exit(1);
        }

        uint8_t recv_ttl = buffer[8];

        t_icmp_packet response_packet;

        // Copy the ICMP packet into our response_packet struct
        memcpy(&response_packet, buffer + ICMP4_FRAME_SIZE, bytes_received - ICMP4_FRAME_SIZE);

        response_packet.size = bytes_received - ICMP4_FRAME_SIZE - 1;

        uint16_t received_checksum = response_packet.checksum;
        response_packet.checksum = 0;

        compute_icmp_checksum(&response_packet);

        if (received_checksum != response_packet.checksum) {
            fprintf(stderr, "ft_ping: warning: remote host returned an invalid checksum (got: %d, expected: %d)\n", received_checksum, response_packet.checksum);
            // Allow the program to continue, having a checksum mismatch is not a fatal error but it must be reported
        }
        float response_time = (get_timestamp() - before) / 1000.0;
        g_summary.delays[g_summary.received - 1] = response_time;
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", response_packet.size, argv[1], response_packet.sequence_number, recv_ttl, response_time);
        if (response_time < g_summary.min_delay) {
            g_summary.min_delay = response_time;
        }
        if (response_time > g_summary.max_delay) {
            g_summary.max_delay = response_time;
        }
        usleep(DEFAULT_MIN_DELAY);
        echo_request.sequence_number++;
    }
    return (0);
}
