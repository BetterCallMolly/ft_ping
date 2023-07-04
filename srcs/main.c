#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

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
    // Print the IP
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);
    printf("PING %s (%s) 56(84) bytes of data.\n", argv[1], ip);
}
