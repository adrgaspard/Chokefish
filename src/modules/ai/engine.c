#include <stdio.h>
#include "../core/logging.h"
#include "engine.h"

void start_search_infinite_time(game_data *game_data, search_result *result)
{
    (void)result;
    (void)game_data;
    printf(FG_YELLOW "NotImplemented: start_search_infinite_time []" COLOR_RESET "\n");
    // TODO
}

void start_search_defined_time(game_data *game_data, search_result *result, uint64_t time_ms)
{
    (void)result;
    (void)game_data;
    printf(FG_YELLOW "NotImplemented: start_search_defined_time [time_ms=%lu]" COLOR_RESET "\n", time_ms);
    // TODO
}

void start_search_incremental_time(game_data *game_data, search_result *result, uint64_t white_remaining_time_ms, uint64_t white_increment_time_ms,  uint64_t black_remaining_time_ms, uint64_t black_increment_time_ms)
{
    (void)result;
    (void)game_data;
    printf(FG_YELLOW "NotImplemented: start_search_incremental_time [white_remaining_time_ms=%lu,white_increment_time_ms=%lu,black_remaining_time_ms=%lu,black_increment_time_ms=%lu]"
        COLOR_RESET "\n", white_remaining_time_ms, white_increment_time_ms, black_remaining_time_ms, black_increment_time_ms);
    // TODO
}

void start_search_time_control(game_data *game_data, search_result *result, uint64_t white_remaining_time_ms, uint64_t black_remaining_time_ms, uint64_t moves_to_go)
{
    (void)result;
    (void)game_data;
    printf(FG_YELLOW "NotImplemented: start_search_incremental_time [white_remaining_time_ms=%lu,black_remaining_time_ms=%lu,moves_to_go=%lu]"
        COLOR_RESET "\n", white_remaining_time_ms, black_remaining_time_ms, moves_to_go);
    // TODO
}

void cancel_search()
{
    printf(FG_YELLOW "NotImplemented: cancel_search []" COLOR_RESET "\n");
    // TODO
}

void reset_engine_cache()
{
    printf(FG_YELLOW "NotImplemented: reset_engine_cache []" COLOR_RESET "\n");
    // TODO
}