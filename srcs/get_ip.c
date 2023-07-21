#include "ft_ping.h"

#define IP_CHARSET "0123456789."
#define MAX_IP_SIZE 15

/**
 * @brief Check if the given string could be an IPv4 address
 * 
 * @param arg String to check
 * @return true If the string could be an IPv4 address, false otherwise
 */
bool _is_ip(char *arg)
{
    register int i = 0;
    while (arg[i] && i < MAX_IP_SIZE)
    {
        if (strchr(IP_CHARSET, arg[i]) == NULL)
            return false;
        i++;
    }
    return (i == MAX_IP_SIZE && arg[i] == '\0');
}

/**
 * @brief Parses an IPv4 address passed as a string
 * 
 * @param arg IPv4 address to parse
 * @param output Output `sockaddr_in` structure to store the parsed IPv4 address
 * @return true if the IPv4 address was successfully parsed, false otherwise
 */
bool _parse_ip_address(char *arg, struct sockaddr_in *output)
{
    int ret = inet_pton(AF_INET, arg, &(output->sin_addr));
    if (ret == -1) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: %s: %s\n", arg, error);
    } else if (ret == 0) {
        fprintf(stderr, "ft_ping: %s: Invalid IP address\n", arg);
    }
    return (ret == 1);
}

bool get_ip(char *arg, struct sockaddr_in *output)
{
    const bool is_ip_addr = _is_ip(arg);
    if (is_ip_addr)
        return _parse_ip_address(arg, output);
    // Our IP is probably a hostname, so we will try to resolve it
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    int ret = getaddrinfo(arg, NULL, &hints, &result);
    if (ret != 0) {
        fprintf(stderr, "ft_ping: '%s': %s\n", arg, gai_strerror(ret));
        return false;
    }
    memcpy(output, result->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(result);
    return true;
}