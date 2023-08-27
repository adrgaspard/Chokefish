#ifndef ENHANCED_TIME_H
#define ENHANCED_TIME_H

#include <stdint.h>
#include <time.h>

static inline uint64_t get_current_uptime();

static inline uint64_t get_current_uptime()
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (uint64_t)current_time.tv_sec * 1000ULL + (uint64_t)current_time.tv_nsec / 1000000ULL;
}

#endif // ENHANCED_TIME_H
