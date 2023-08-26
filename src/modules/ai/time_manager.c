#include "time_manager.h"

uint64_t get_search_time_from_incremental(uint64_t white_remaining_time_ms, uint64_t white_increment_time_ms, uint64_t black_remaining_time_ms, uint64_t black_increment_time_ms)
{
    (void)white_remaining_time_ms;
    (void)white_increment_time_ms;
    (void)black_remaining_time_ms;
    (void)black_increment_time_ms;
    // TODO
    return 1000;
}

uint64_t get_search_time_from_control(uint64_t white_remaining_time_ms, uint64_t black_remaining_time_ms, uint64_t moves_to_go)
{
    (void)white_remaining_time_ms;
    (void)black_remaining_time_ms;
    (void)moves_to_go;
    // TODO
    return 1000;
}

uint64_t get_search_time_after_pondering(uint64_t previous_time_in_ms, uint64_t ponder_time_in_ms)
{
    return previous_time_in_ms - (ponder_time_in_ms / 2);
}