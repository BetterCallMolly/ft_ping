#include <stdint.h>
#include <sys/time.h>
#include <stddef.h>

/**
 * @brief Uses gettimeofday to get the current timestamp in microseconds
 * 
 * @return uint32_t The current timestamp in microseconds
 */
uint32_t get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}