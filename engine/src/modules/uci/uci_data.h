#ifndef UCI_DATA_H
#define UCI_DATA_H

#include "../game_tools/types.h"
#include "../serialization/consts.h"
#include "consts.h"

extern move_generation_options g_all_moves;
extern char g_command_str[COMMAND_STR_LEN + 1];
extern char g_current_fen[FEN_LENGTH_UPPER_BOUND + 1];
extern char g_current_moves[COMMAND_STR_LEN + 1];
extern bool g_debug;
extern pthread_t g_debug_thread_id;
extern engine_options g_options;
extern engine_state g_state;
extern search_result g_search_result;

void initialize_or_reset_uci_data();

#endif // UCI_DATA_H
