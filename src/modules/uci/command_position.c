#include <stdio.h>
#include <string.h>
#include "../ai/engine.h"
#include "../core/board.h"
#include "../core/move_generation_result.h"
#include "../core/move_generator.h"
#include "../game_tools/game_data.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "commands.h"
#include "consts.h"
#include "debug_printer.h"
#include "engine_state.h"

move_generation_options s_all_moves = { .include_quiet_moves = true, .promotion_types_to_include = PROMOTION_ALL };

void handle_position_command(char *cmd, char *edit_cmd, uint64_t start_index, engine_state *state, game_data *game_data, char *current_fen, char *current_moves)
{
    char fen[FEN_LENGTH_UPPER_BOUND + 1], *command_args, *moves_ptr, *fen_ptr, *new_moves_ptr, *current_move_ptr;
    uint64_t moves_opt_len, fen_part_len, fen_opt_static_len, uci_delimiter_static_len, new_moves_index;
    move_generation_result generation_result;
    move current_move;
    bool is_new_game;
    if (is_waiting_for_setup(*state) || is_waiting_for_ready(*state))
    {
        return;
    }
    start_index += strlen(edit_cmd) + 1;
    if (strlen(cmd) <= start_index)
    {
        return;
    }
    uci_delimiter_static_len = strlen(UCI_DELIMITER);
    fen_opt_static_len = strlen(GE_CMD_POSITION_OPT_FEN UCI_DELIMITER);
    moves_ptr = NULL;
    moves_opt_len = 0;
    new_moves_index = 0;
    new_moves_ptr = NULL;
    command_args = edit_cmd + start_index;
    if (strstr(command_args, GE_CMD_POSITION_OPT_MOVES) != NULL)
    {
        moves_ptr = strstr(command_args, UCI_DELIMITER GE_CMD_POSITION_OPT_MOVES UCI_DELIMITER);
        if (moves_ptr == NULL)
        {
            return;
        }
        moves_opt_len = strlen(UCI_DELIMITER GE_CMD_POSITION_OPT_MOVES UCI_DELIMITER);
        if (strlen(moves_ptr) <= moves_opt_len)
        {
            return;
        }
        moves_ptr += moves_opt_len;
    }
    fen_part_len = moves_ptr == NULL ? strlen(command_args) : (uint64_t)moves_ptr - (uint64_t)command_args - moves_opt_len;
    if (strncmp(command_args, GE_CMD_POSITION_OPT_STARTPOS, fen_part_len) == 0)
    {
        strcpy(fen, START_FEN_STR);
    }
    else
    {
        if (strncmp(command_args, GE_CMD_POSITION_OPT_FEN UCI_DELIMITER, fen_opt_static_len) != 0)
        {
            return;
        }
        fen_ptr = command_args + fen_opt_static_len;
        strncpy(fen, fen_ptr, fen_part_len - fen_opt_static_len);
        fen[fen_part_len - fen_opt_static_len] = '\0';
    }
    if (strcmp(current_fen, fen) != 0)
    {
        is_new_game = true;
        strcpy(current_fen, fen);
        strcpy(current_moves, moves_ptr == NULL ? "" : moves_ptr);
    }
    else
    {
        while (true)
        {
            if (moves_ptr[new_moves_index] == current_moves[new_moves_index])
            {
                if (current_moves[new_moves_index] == '\0')
                {
                    is_new_game = false;
                    break;
                }
            }
            else
            {
                is_new_game = current_moves[new_moves_index] != '\0';
                strcpy(current_moves, moves_ptr == NULL ? "" : moves_ptr);
                if (moves_ptr != NULL && strlen(moves_ptr) > new_moves_index)
                {
                    new_moves_ptr = moves_ptr + new_moves_index;
                    if (strstr(new_moves_ptr, UCI_DELIMITER) == new_moves_ptr)
                    {
                        new_moves_ptr = strlen(new_moves_ptr) > uci_delimiter_static_len ? new_moves_ptr + uci_delimiter_static_len : NULL;
                    }
                }
                break;
            }
            new_moves_index++;
        }
    }
    if (is_new_game)
    {
        handle_ucinewgame_command(state);
        reset_game_data(game_data, fen);
    }
    if (new_moves_ptr == NULL)
    {
        while (game_data->move_stack.count > 0)
        {
            undo_move(&(game_data->board), peek_from_move_stack(&(game_data->move_stack)), false);
        }
        new_moves_ptr = moves_ptr;
    }
    if (new_moves_ptr != NULL)
    {
        current_move_ptr = strtok(new_moves_ptr, UCI_DELIMITER);
        reset_move_generation_result(&(generation_result));
        while (current_move_ptr != NULL)
        {
            generate_moves(&(game_data->board), &(generation_result), s_all_moves);
            current_move = move_data_to_existing_moves(move_data_from_string(current_move_ptr), generation_result.moves, generation_result.moves_count);
            if (is_movement_empty(current_move))
            {
                fprintf(stderr, "Error: unknown move '%s' provided in input!", current_move_ptr);
                return;
            }
            do_move(&(game_data->board), current_move, false);
            current_move_ptr = strtok(NULL, UCI_DELIMITER);
        }
    }
    if (is_working(*state))
    {
        on_cancelling_work(state);
        cancel_search();
    }
    disable_debug_printing();
}
