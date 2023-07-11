#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdbool.h>
#include "icmp.h"

#ifndef DEFAULT_DATA_SIZE
    #define DEFAULT_DATA_SIZE 56
#endif