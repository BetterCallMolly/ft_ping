#include <options.h>
#include <stdio.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void options_init(t_options *options)
{
    options->count = 0;                    // never stops at 0
    options->interval = DEFAULT_MIN_DELAY; // number of seconds between each ICMP_ECHO
    options->numeric = 0;                  // resolve hostnames
    options->verbose = 0;                  // print more info
    options->timeout = DEFAULT_TIMEOUT;    // number of seconds before timeout
    options->linger = 0;                   // number of seconds to wait for a response
    options->size = 0;                     // size of the ICMP_ECHO payload
    options->help = 0;                     // show usage (overrides all other options, since the program will exit)
    bzero(options->hostname, 255);
}

/**
 * @brief Parses a numeric option passed as a string.
 * 
 * @param field The field to store the parsed value in (must be in a s_options struct)
 * @param s String to parse
 * @param min Minimum value for the field
 * @param max Maximum value for the field
 * @return true If the option was parsed successfully, false otherwise
 */
bool parse_numeric_option(void *field, char *s, uint8_t min, uint64_t max)
{
    uint64_t val = 0;
    while (*s)
    {
        if (*s < '0' || *s > '9')
            return false;
        val = val * 10 + (*s - '0');
        if (val > max)
            return false;
        s++;
    }
    if (val < min)
    {
        fprintf(stderr, "%s: expected a value between %u and %lu\n", PROGRAM_NAME, min, max);
        return false;
    }
    switch (sizeof(field))
    {
        case 8:
            *(uint64_t *)field = val;
            return true;
        case 4:
            *(uint32_t *)field = val;
            return true;
        default:
            return false;
    }
}

/**
 * @brief Parses a short opiton (e.g. -c) passed as a string.
 * 
 * @param field The field to store the parsed value in
 * @param s The (next) argument, which should be the value
 * @param min Minimum value for the field (if applicable)
 * @param max Maximum value for the field (if applicable)
 * @param is_numeric Whether the option is numeric (e.g. -c 5)
 * @return true If the option was parsed successfully, false otherwise 
 */
bool parse_short_option(void *field, char *value, uint8_t min, uint64_t max, bool is_numeric)
{
    // If the option is boolean, the next parameter starts with a '-' or is the hostname, in these to case, set the value to true
    if (!is_numeric)
    {
        *(bool *)field = true;
        return true;
    }
    if (is_numeric)
    {
        if (!value) // We're expecting a number in value
        {
            fprintf(stderr, "%s: option requires an argument\n", PROGRAM_NAME);
            return false;
        }
        if (!parse_numeric_option(field, value, min, max)) {
            fprintf(stderr, "%s: passed value is not a valid number\n", PROGRAM_NAME);
            return false;
        }
    }
    return true;
}

void options_parse(t_options *options, int argc, char **argv)
{
    // only support short options for now
    int offset = 1;
    while (offset < argc)
    {
        if (argv[offset][0] == '-' && strlen(argv[offset]) != 2) {
            fprintf(stderr, "%s: invalid option -- '%s'\n", PROGRAM_NAME, argv[offset]);
            exit(3);
        }
        switch (argv[offset][1])
        {
            case 'c':
                offset++;
                if (!parse_short_option(&options->count, argv[offset], 0, UINT64_MAX, true))
                    exit(4);
                break;
            case 'i':
                offset++;
                if (!parse_short_option(&options->interval, argv[offset], 0, UINT32_MAX, true))
                    exit(4);
                break;
            case 'n':
                if (!parse_short_option(&options->numeric, argv[offset], 0, 0, false))
                    exit(4);
                break;
            case 'v':
                if (!parse_short_option(&options->verbose, argv[offset], 0, 0, false))
                    exit(4);
                break;
            case 'w':
                offset++;
                if (!parse_short_option(&options->timeout, argv[offset], 0, UINT32_MAX, true))
                    exit(4);
                break;
            case 'l':
                offset++;
                if (!parse_short_option(&options->linger, argv[offset], 0, UINT32_MAX, true))
                    exit(4);
                break;
            case 's':
                offset++;
                if (!parse_short_option(&options->size, argv[offset], 0, UINT32_MAX, true))
                    exit(4);
                break;
            case 'h':
                if (!parse_short_option(&options->help, argv[offset], 0, 0, false))
                    exit(4);
                break;
            default:
            {
                // if we're are NOT on the last argument, error
                if (offset != argc - 1)
                {
                    fprintf(stderr, "%s: invalid option -- '%s'\n", PROGRAM_NAME, argv[offset]);
                    exit(3);
                }
                // otherwise, we're on the last argument, which should be the hostname, copy it
                strncpy(options->hostname, argv[offset], 255);
            }
        }
        ++offset;
    }
}

void show_usage(void)
{
    printf(
            "Usage: %s [OPTION...] HOST\n" \
            "Send ICMP_ECHO requests to network hosts.\n" \
            "\n" \
            "  -c, --count=N      stop after sending N packets (min: %u, max: %lu)\n" \
            "  -i, --interval=N   wait N seconds between sending each packet (min: %u, max: %u)\n" \
            "  -n, --numeric      do not resolve hostnames (default: false)\n" \
            "  -v, --verbose      print more information (default: false)\n" \
            "  -w, --timeout=N    wait N seconds for a response (min: %u, max: %u)\n" \
            "  -l, --linger=N     wait N seconds for responses after all packets have been sent (min: %u, max: %u)\n" \
            "  -s, --size=N       use N bytes of data in ICMP_ECHO packets (min: %u, max: %u)\n" \
            "  -h, --help         display this help and exit\n" \
        , PROGRAM_NAME, 0, UINT64_MAX, 0, UINT32_MAX, 0, UINT32_MAX, 0, UINT32_MAX, 0, UINT32_MAX
    );
}

void print_options(t_options *options)
{
    printf(
            "count: %lu\n" \
            "interval: %u\n" \
            "numeric: %d\n" \
            "verbose: %d\n" \
            "timeout: %u\n" \
            "linger: %u\n" \
            "size: %u\n" \
            "help: %d\n" \
            "host: %s\n"
        , options->count, options->interval, options->numeric, options->verbose, options->timeout, options->linger, options->size, options->help, options->hostname
    );
}