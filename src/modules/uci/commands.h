#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>
#include <stdint.h>
#include "../ai/types.h"
#include "../game_tools/types.h"
#include "types.h"

void handle_uci_command(engine_state *state, engine_options options);
void handle_debug_command(char *edit_cmd, bool *debug, engine_state state, search_result *result);
void handle_isready_command(engine_state *state);
void handle_setoption_command(char *edit_cmd, engine_options *options);
void handle_ucinewgame_command(engine_state *state);
void handle_position_command(char *cmd, char *edit_cmd, uint64_t start_index, engine_state *state, game_data *game_data, char *current_fen, char *current_moves);
void handle_go_command(char *edit_cmd, engine_state *state, game_data *game_data, search_result *search_result, bool debug);
void handle_stop_command(engine_state *state, search_result *result);
void handle_ponderhit_command(engine_state *state);
void handle_quit_command();

#endif // COMMAND_UCI_H
