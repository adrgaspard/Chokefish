#include <time.h>
#include "enhanced_time.h"

uint64_t get_current_uptime()
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (uint64_t)current_time.tv_sec * 1000ULL + (uint64_t)current_time.tv_nsec / 1000000ULL;
}
