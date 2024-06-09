#ifndef UCI_DATA_H
#define UCI_DATA_H

#include "../game_tools/types.h"
#include "../serialization/consts.h"
#include "consts.h"

move_generation_options g_all_moves = { .include_quiet_moves = true, .promotion_types_to_include = PROMOTION_ALL };

char g_command_str[COMMAND_STR_LEN + 1];
char g_current_fen[FEN_LENGTH_UPPER_BOUND + 1];
char g_current_moves[COMMAND_STR_LEN + 1];

bool g_debug;
pthread_t g_debug_thread_id;

engine_options g_options;
engine_state g_state;
search_result g_search_result;

void initialize_or_reset_uci_data();

#endif // UCI_DATA_H
