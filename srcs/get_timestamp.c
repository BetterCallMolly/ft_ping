#include <stdint.h>
#include <sys/time.h>
#include <stddef.h>

uint32_t get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}