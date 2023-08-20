#ifndef ENGINE_H
#define ENGINE_H

#include "../game_tools/types.h"
#include "types.h"

void start_search_infinite_time(game_data *game_data, search_result *result);
void start_search_defined_time(game_data *game_data, search_result *result, uint64_t time_ms);
void start_search_incremental_time(game_data *game_data, search_result *result, uint64_t white_remaining_time_ms, uint64_t white_increment_time_ms,  uint64_t black_remaining_time_ms, uint64_t black_increment_time_ms);
void start_search_time_control(game_data *game_data, search_result *result, uint64_t white_remaining_time_ms, uint64_t black_remaining_time_ms, uint64_t moves_to_go);
void cancel_search();
void reset_engine_cache();

#endif // ENGINE_H
