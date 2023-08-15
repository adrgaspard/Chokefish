#include <stdio.h>
#include <string.h>
#include "../core/board.h"
#include "../core/move_generation_result.h"
#include "../core/move_generator.h"
#include "../game_tools/game_data.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "uci.h"
#include "engine_options.h"
#include "engine_state.h"

#define COMMAND_STR_LEN 80000

static char s_command_str[COMMAND_STR_LEN + 1];
static char s_edit_command_str[COMMAND_STR_LEN + 1];
static char s_current_fen[FEN_LENGTH_UPPER_BOUND + 1];
static char s_current_moves[COMMAND_STR_LEN + 1];
static engine_options s_options;
static engine_state s_state;
static bool s_debug;
static game_data s_master_game_data;
static move_generation_options s_all_moves = { .include_quiet_moves = true, .promotion_types_to_include = PROMOTION_ALL };

static void handle_uci_command();
static void handle_debug_command(char *current_cmd);
static void handle_isready_command();
static void handle_setoption_command(char *current_cmd);
static void handle_ucinewgame_command();
static void handle_position_command(char *current_cmd, uint64_t start_index);
static void handle_quit_command();

void handle_commands()
{
    char *current_cmd;
    uint64_t start_index;
    s_command_str[0] = '\0';
    s_edit_command_str[0] = '\0';
    s_current_fen[0] = '\0';
    s_current_moves[0] = '\0';
    s_state = get_default_state();
    s_options = get_default_options();
    s_debug = false;
    while (true)
    {
        start_index = 0;
        current_cmd = NULL;
        if (fgets(s_command_str, COMMAND_STR_LEN, stdin) == NULL)
        {
            break;
        }
        s_command_str[strlen(s_command_str) - 1] = '\0';
        strcpy(s_edit_command_str, s_command_str);
        current_cmd = strtok(s_edit_command_str, UCI_DELIMITER);
        while (current_cmd != NULL)
        {
            if (strcmp(current_cmd, GE_CMD_UCI) == 0)
            {
                handle_uci_command();
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_DEBUG) == 0)
            {
                handle_debug_command(current_cmd);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_ISREADY) == 0)
            {
                handle_isready_command();
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_SETOPTION) == 0)
            {
                handle_setoption_command(current_cmd);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_UCINEWGAME) == 0)
            {
                handle_ucinewgame_command();
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_POSITION) == 0)
            {
                handle_position_command(current_cmd, start_index);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_GO) == 0)
            {
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_STOP) == 0)
            {
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_PONDERHIT) == 0)
            {
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_QUIT) == 0)
            {
                handle_quit_command();
                break;
            }
            else
            {
                start_index += strlen(current_cmd) + 1;
                current_cmd = strtok(NULL, UCI_DELIMITER);
            }
        }
    }
}

static void handle_uci_command()
{
    if (is_waiting_for_setup(s_state))
    {
        // Identifying engine
        printf(EG_CMD_ID " " EG_CMD_ID_OPT_NAME " " ENGINE_NAME "\n");
        printf(EG_CMD_ID " " ENGINE_AUTHOR " " ENGINE_AUTHOR "\n");

        // Sending options
        print_options(s_options);

        // Actualize state
        on_sending_uciok(&s_state);
        printf(EG_CMD_UCIOK "\n");
    }
}

static void handle_debug_command(char *current_cmd)
{  
    current_cmd = strtok(NULL, UCI_DELIMITER);
    if (current_cmd != NULL)
    {
        if (strcmp(current_cmd, GE_CMD_DEBUG_OPT_ON) == 0)
        {
            s_debug = true;
        }
        else if (strcmp(current_cmd, GE_CMD_DEBUG_OPT_OFF) == 0)
        {
            s_debug = false;
        }
    }
}

static void handle_isready_command()
{
    if (!is_waiting_for_setup(s_state))
    {
        on_sending_readyok(&s_state);
        printf(EG_CMD_READYOK "\n");
    }
}

static void handle_setoption_command(char *current_cmd)
{
    char name[OPT_NAME_MAX_LENGTH + 1], value[OPT_VALUE_MAX_LENGHT + 1];
    uint64_t value_len;
    value[0] = '\0';
    current_cmd = strtok(NULL, UCI_DELIMITER);
    if (current_cmd == NULL || strcmp(current_cmd, GE_CMD_SETOPTION_OPT_NAME) != 0)
    {
        return;
    }
    current_cmd = strtok(NULL, UCI_DELIMITER);
    if (current_cmd == NULL || strlen(current_cmd) > OPT_NAME_MAX_LENGTH)
    {
        return;
    }
    else
    {
        strcpy(name, current_cmd);
    }
    current_cmd = strtok(NULL, UCI_DELIMITER);
    if (current_cmd == NULL || strcmp(current_cmd, GE_CMD_SETOPTION_OPT_VALUE) != 0)
    {
        return;
    }
    current_cmd = strtok(NULL, UCI_DELIMITER);
    if (current_cmd == NULL)
    {
        return;
    }
    value_len = 0;
    while (current_cmd != NULL)
    {
        value_len += strlen(current_cmd);
        if (value_len > OPT_VALUE_MAX_LENGHT)
        {
            return;
        }
        strcat(value, current_cmd);
        current_cmd = strtok(NULL, UCI_DELIMITER);
        if (current_cmd != NULL)
        {
            value_len += strlen(UCI_DELIMITER);
            if (value_len > OPT_VALUE_MAX_LENGHT)
            {
                return;
            }
            strcat(value, UCI_DELIMITER);
        }
    }
    setoption(&(s_options), name, value);
}

static void handle_ucinewgame_command()
{
    if (!is_waiting_for_setup(s_state) && !is_waiting_for_ready(s_state))
    {
        if (is_working(s_state))
        {
            on_cancelling_work(&s_state);
            // TODO: cancel ponder/search without returning anything
        }
        // TODO: clear all search cache, put the state on idling
    }
}

static void handle_position_command(char *current_cmd, uint64_t start_index)
{
    char fen[FEN_LENGTH_UPPER_BOUND + 1], *command_args, *moves_ptr, *fen_ptr, *new_moves_ptr, *current_move_ptr;
    uint64_t moves_opt_len, fen_part_len, fen_opt_static_len, uci_delimiter_static_len, new_moves_index;
    move_generation_result generation_result;
    move current_move;
    bool is_new_game;
    uci_delimiter_static_len = strlen(UCI_DELIMITER);
    fen_opt_static_len = strlen(GE_CMD_POSITION_OPT_FEN UCI_DELIMITER);
    moves_ptr = NULL;
    moves_opt_len = 0;
    new_moves_index = 0;
    new_moves_ptr = NULL;
    start_index += strlen(current_cmd) + 1;
    if (is_waiting_for_setup(s_state) || is_waiting_for_ready(s_state))
    {
        return;
    }
    if (strlen(s_command_str) <= start_index)
    {
        return;
    }
    command_args = s_edit_command_str + start_index;
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
    if (strcmp(s_current_fen, fen) != 0)
    {
        is_new_game = true;
        strcpy(s_current_fen, fen);
        strcpy(s_current_moves, moves_ptr == NULL ? "" : moves_ptr);
    }
    else
    {
        while (true)
        {
            if (moves_ptr[new_moves_index] == s_current_moves[new_moves_index])
            {
                if (s_current_moves[new_moves_index] == '\0')
                {
                    is_new_game = false;
                    break;
                }
            }
            else
            {
                is_new_game = s_current_moves[new_moves_index] != '\0';
                strcpy(s_current_moves, moves_ptr == NULL ? "" : moves_ptr);
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
        handle_ucinewgame_command();
        reset_game_data(&s_master_game_data, fen);
    }
    if (new_moves_ptr == NULL)
    {
        while (s_master_game_data.move_stack.count > 0)
        {
            undo_move(&(s_master_game_data.board), peek_from_move_stack(&(s_master_game_data.move_stack)), false);
        }
        new_moves_ptr = moves_ptr;
    }
    if (new_moves_ptr != NULL)
    {
        current_move_ptr = strtok(new_moves_ptr, UCI_DELIMITER);
        reset_move_generation_result(&(generation_result));
        while (current_move_ptr != NULL)
        {
            generate_moves(&(s_master_game_data.board), &(generation_result), s_all_moves);
            current_move = move_data_to_existing_moves(move_data_from_string(current_move_ptr), generation_result.moves, generation_result.moves_count);
            if (is_movement_empty(current_move))
            {
                fprintf(stderr, "Error: unknown move '%s' provided in input!", current_move_ptr);
                return;
            }
            do_move(&(s_master_game_data.board), current_move, false);
            current_move_ptr = strtok(NULL, UCI_DELIMITER);
        }
    }
    if (is_working(s_state))
    {
        on_cancelling_work(&s_state);
        // TODO: cancel ponder/search without returning anything
    }
    if (s_options.ponder)
    {
        // TODO: register on_starting_ponder in engine_state.h
        // TODO: start pondering
    }
}

static void handle_quit_command()
{
    // TODO: kill all existing threads
    exit(EXIT_SUCCESS);
}
