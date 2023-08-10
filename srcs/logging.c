#include "ft_ping.h"

/**
 * @brief Logs the summary of the ping session before starting
 * 
 * @note Format : PING <host> (<ip>) <size> bytes of data.
 */
void preping_info(char *host, char *ip, uint16_t size)
{
    printf("PING %s (%s): %d data bytes.\n", host, ip, size);
}

/**
 * @brief Logs the summary of the ping session after it ends
 * 
 * @note Format : --- <host> ping statistics ---
 *               : <sent> packets transmitted, <received> received, <loss> packet loss
 *               : round-trip min/avg/max/stddev = <min>/<avg>/<max>/<stddev> ms
 */
void postping_info(t_summary *summary)
{
    printf("--- %s ping statistics ---\n", summary->HOST_NAME);
    printf("%d packets transmitted, %d received, %.2f%% packet loss\n",
        summary->sent,
        summary->received,
        (summary->received == 0) ? 100 : (float)(summary->sent - summary->received) / (float)summary->sent * 100
    );
    float avg_delay = avg(summary);
    float standard_dev = stddev(summary, avg_delay);
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
        (summary->min_delay == 2147483647) ? 0 : summary->min_delay,
        (avg_delay != avg_delay) ? 0 : avg_delay,
        summary->max_delay,
        (standard_dev != standard_dev) ? 0 : standard_dev
    );
}

void ping_info(
    t_summary *summary,
    t_icmp_packet *response_packet,
    uint8_t ttl,
    float response_time,
    struct sockaddr_in *sa
)
{
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
        response_packet->size,
        inet_ntoa(sa->sin_addr),
        response_packet->sequence_number,
        ttl,
        response_time
    );
    // Update summary
    if (response_time < summary->min_delay) {
        summary->min_delay = response_time;
    }
    if (response_time > summary->max_delay) {
        summary->max_delay = response_time;
    }
    summary->delays[summary->received - 1] = response_time;
}