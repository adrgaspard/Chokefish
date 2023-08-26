#ifndef ENHANCED_TIME_H
#define ENHANCED_TIME_H

#include <stdint.h>
#include <time.h>

static inline uint64_t get_current_uptime();

static inline uint64_t get_current_uptime()
{
    return (uint64_t)(clock() / (CLOCKS_PER_SEC / 1000));
}

#endif // ENHANCED_TIME_H
