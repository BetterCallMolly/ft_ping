#include "ft_ping.h"

static t_summary g_summary = {0};

void sigint_handler(int signum) {
    (void) signum;
    postping_info(&g_summary);
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
        fprintf(stderr, "ft_ping: missing host operand\nTry 'ft_ping --help' for more information.\n");
        exit(1);
    }
    t_options options;
    options_init(&options);
    options_parse(&options, argc, argv);
    if (options.help) {
        show_usage();
        exit(0);
    }
    struct sockaddr_in sa = {0};
    if (!get_ip(options.hostname, &sa))
        return (1);

    // At this point, we have a valid IP address
    t_icmp_packet echo_request = generate_base_packet(DEFAULT_DATA_SIZE);

    // Setup signal handlers
    init_summary(options.hostname);
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, ping_timeout);

    // Create a raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: socket: %s\n", error);
        exit(1);
    }

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = DEFAULT_TIMEOUT;
    timeout.tv_usec = 0;

    // use setsockopt() to request timeout on recv()
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: setsockopt: %s\n", error);
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

    // const size_t send_size = EMPTY_PACKET_SIZE + echo_request.size;
    const size_t receive_size = ICMP4_FRAME_SIZE + EMPTY_PACKET_SIZE + echo_request.size;

    preping_info(options.hostname, inet_ntoa(sa.sin_addr), DEFAULT_DATA_SIZE);
    // Surround the below block with a loop to send multiple packets (for now, we'll only send one);
    while (1) {
        // Push our timestamp in the packet
        echo_request.checksum = 0;
        compute_icmp_checksum(&echo_request);

        float before = get_timestamp();
        ssize_t bytes_sent = send_packet(&echo_request, sockfd, &dest);
        if (bytes_sent < 0) {
            char *error = strerror(errno);
            fprintf(stderr, "ft_ping: sendto: %s\n", error);
            exit(1);
        }
        g_summary.sent++;
        // Receive packet
        uint8_t buffer[IP_MAXPACKET];
        ssize_t bytes_received = receive_packet(sockfd, buffer, receive_size);
        if (bytes_received < 0) {
            close(sockfd);
            exit(1); // Typically, recv errors are fatal, it's not worth retrying
        } else if (bytes_received == 0) {
            g_summary.lost++;
            continue;
        }
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
        memcpy(&response_packet, buffer + ICMP4_FRAME_SIZE, bytes_received - ICMP4_FRAME_SIZE);
        response_packet.size = bytes_received - ICMP4_FRAME_SIZE - 1;

        compare_checksum(&response_packet);

        float response_time = (get_timestamp() - before) / 1000.0;
        ping_info(&g_summary, &response_packet, recv_ttl, response_time, &dest);
        usleep(DEFAULT_MIN_DELAY);
        echo_request.sequence_number++;
    }
    return (0);
}
