#include <stdint.h>
#include <stdbool.h>

#ifndef DEFAULT_DATA_SIZE
    #define DEFAULT_DATA_SIZE 56
#endif

#ifndef DEFAULT_MIN_DELAY
    #define DEFAULT_MIN_DELAY 1000000
#endif

#ifndef MAX_ARG_SIZE
    #define MAX_ARG_SIZE 2000000 // 2MB
#endif

#ifndef MAX_SAMPLE_SIZE
    #define MAX_SAMPLE_SIZE 1000 // over is just stupid
#endif

#ifndef DEFAULT_TIMEOUT
    #define DEFAULT_TIMEOUT 3 // 3 seconds
#endif

#ifndef PROGRAM_NAME
    #define PROGRAM_NAME "ft_ping"
#endif

typedef struct s_options {
    uint64_t    count;     // -c, --count
    uint32_t    interval;  // -i, --interval
    bool        numeric;   // -n, --numeric
    bool        verbose;   // -v, --verbose
    uint32_t    timeout;   // -w, --timeout
    uint32_t    linger;    // -l, --linger
    bool        help;      // -h, --help, -?
    uint16_t    size;      // -s, --size
    char        hostname[255]; // positional argument (last argument)
} t_options;

void options_init(t_options *options);
void options_parse(t_options *options, int argc, char **argv);
void show_usage(void);
void print_options(t_options *options);