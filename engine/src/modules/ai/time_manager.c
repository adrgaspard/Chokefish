#include "consts.h"
#include "time_manager.h"

#define SEARCH_TIME_DIVIDER 180

static inline uint64_t get_clamped_time(uint64_t time, uint64_t min_time, uint64_t max_time);

uint64_t get_search_time_from_incremental(board *board, uint64_t white_remaining_time_ms, uint64_t white_increment_time_ms, uint64_t black_remaining_time_ms, uint64_t black_increment_time_ms)
{
    uint64_t remaining_time, increment, search_time;
    remaining_time = board->color_to_move == COLOR_WHITE ? white_remaining_time_ms : black_remaining_time_ms;
    increment = board->color_to_move == COLOR_WHITE ? white_increment_time_ms : black_increment_time_ms;
    search_time = remaining_time / SEARCH_TIME_DIVIDER;
    if (search_time < increment)
    {
        search_time = increment;
    }
    return get_clamped_time(search_time, MIN_SEARCH_TIME_IN_MS, MAX_SEARCH_TIME_IN_MS);
}

uint64_t get_search_time_from_control(board *board, uint64_t white_remaining_time_ms, uint64_t black_remaining_time_ms, uint64_t moves_to_go)
{
    uint64_t remaining_time, search_time;
    remaining_time = board->color_to_move == COLOR_WHITE ? white_remaining_time_ms : black_remaining_time_ms;
    search_time = remaining_time / (moves_to_go + 1);
    if (2 * search_time < (remaining_time / moves_to_go))
    {
        search_time += search_time / 2;
    }
    return get_clamped_time(search_time, MIN_SEARCH_TIME_IN_MS, MAX_SEARCH_TIME_IN_MS);
}

uint64_t get_search_time_after_pondering(uint64_t previous_time_in_ms, uint64_t ponder_time_in_ms)
{
    return previous_time_in_ms - (ponder_time_in_ms / 2);
}

static inline uint64_t get_clamped_time(uint64_t time, uint64_t min_time, uint64_t max_time)
{
    return time > max_time ? max_time : (time < min_time ? min_time : time);
}