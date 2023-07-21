#include "ft_ping.h"

/**
 * @brief Sends an ECHO_REQUEST packet to the specified destination
 * 
 * @param packet Packet to send
 * @param sockfd Socket file descriptor
 * @param dest Destination
 * @return ssize_t 
 */
ssize_t send_packet(t_icmp_packet *packet, int sockfd, struct sockaddr_in *dest)
{
    return sendto(
        sockfd,
        serialize_icmp_packet(packet),
        EMPTY_PACKET_SIZE + packet->size,
        0,
        (struct sockaddr *)dest, sizeof(*dest)
    );
}

/**
 * @brief Receives an ECHO_REPLY packet from the specified socket
 * 
 * @param sockfd Socket file descriptor
 * @param buffer Buffer to store the packet
 * @param size Size of the buffer
 * @return ssize_t 
 */
ssize_t receive_packet(int sockfd, uint8_t *buffer, size_t size)
{
    alarm(DEFAULT_TIMEOUT);
    // TODO: Use recvmsg() instead of recvfrom()
    ssize_t bytes_received = recvfrom(
        sockfd,
        buffer,
        size,
        0,
        NULL, NULL
    );
    alarm(0);
    if (bytes_received < 0) {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: recvmsg: %s\n", error);
    }
    return bytes_received;
}