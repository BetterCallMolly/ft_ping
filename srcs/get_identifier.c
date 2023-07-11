#include "ft_ping.h"
#include <sys/time.h>

uint16_t get_echo_identifier(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint16_t)tv.tv_usec & 0x00FF; // I noticed that the identifier is only 1 byte in the original ping, so we'll do the same
}