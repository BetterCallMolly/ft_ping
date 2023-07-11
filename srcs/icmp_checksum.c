#include "ft_ping.h"

/**
 * @brief Compute the checksum of a passed buffer of size n
 * 
 * @note This function is a modern version of RFC 1071's C implementation
 * @param buffer The buffer to compute the checksum of
 * @return uint32_t The computed checksum
 */
int32_t checksum(uint8_t *buffer, uint32_t n)
{
    register int32_t sum = 0;

    /* Iterate through buffer two bytes at a time */
    while (n > 1) {
        sum += *(uint16_t *)buffer;
        buffer += 2;
        n -= 2;
    }

    /* If there's a single byte left over, add it to the sum */
    if (n > 0) {
        sum += *(uint8_t *)buffer;
    }

    /* Fold sum to 16 bits */
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    /* One's complement the result */
    return ~sum;
}