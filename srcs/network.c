#include "ft_ping.h"
#include <errno.h>

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
        (struct sockaddr *)dest, sizeof(*dest));
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
    // alarm(DEFAULT_TIMEOUT);

    struct iovec iov = {
        .iov_base = buffer,
        .iov_len = size
    };

    struct msghdr msg = {
        .msg_name = NULL,
        .msg_namelen = 0,
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = NULL,
        .msg_controllen = 0,
        .msg_flags = 0
    };

    ssize_t bytes_received = recvmsg(sockfd, &msg, 0);

    // alarm(0);

    if (bytes_received < 0 && errno != EAGAIN)
    {
        char *error = strerror(errno);
        fprintf(stderr, "ft_ping: recvmsg: %s\n", error);
    }

    return errno == EAGAIN ? 0 : bytes_received;
}