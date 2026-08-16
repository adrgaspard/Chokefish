#include "enhanced_time.h"

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(_MSC_VER)

#include <windows.h>

uint64_t get_current_uptime()
{
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (uint64_t)(counter.QuadPart * 1000 / frequency.QuadPart);
}

#else

#include <time.h>

uint64_t get_current_uptime()
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (uint64_t)current_time.tv_sec * 1000ULL + (uint64_t)current_time.tv_nsec / 1000000ULL;
}

#endif
