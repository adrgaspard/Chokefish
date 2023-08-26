#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <stdint.h>

uint64_t get_search_time_from_incremental(uint64_t white_remaining_time_ms, uint64_t white_increment_time_ms, uint64_t black_remaining_time_ms, uint64_t black_increment_time_ms);
uint64_t get_search_time_from_control(uint64_t white_remaining_time_ms, uint64_t black_remaining_time_ms, uint64_t moves_to_go);
uint64_t get_search_time_after_pondering(uint64_t previous_time_in_ms, uint64_t ponder_time_in_ms);

#endif // TIME_MANAGER_H
