#include <stdio.h>
#include <string.h>
#include "../ai/search_result.h"
#include "../ai/types.h"
#include "../game_tools/game_data.h"
#include "../serialization/consts.h"
#include "commands.h"
#include "uci.h"
#include "engine_options.h"
#include "engine_state.h"

static engine_options s_options;

static bool s_debug;

static engine_state s_state;

static char s_command_str[COMMAND_STR_LEN + 1];
static char s_edit_command_str[COMMAND_STR_LEN + 1];
static char s_current_fen[FEN_LENGTH_UPPER_BOUND + 1];
static char s_current_moves[COMMAND_STR_LEN + 1];

static game_data s_game_data;
static search_result s_search_result;

static void reset_internal_data();

void handle_commands()
{
    char *current_cmd;
    uint64_t start_index;
    reset_internal_data();
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
                handle_uci_command(&s_state, s_options);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_DEBUG) == 0)
            {
                handle_debug_command(current_cmd, &s_debug, s_state, &s_search_result);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_ISREADY) == 0)
            {
                handle_isready_command(&s_state);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_SETOPTION) == 0)
            {
                handle_setoption_command(current_cmd, &s_options);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_UCINEWGAME) == 0)
            {
                handle_ucinewgame_command(&s_state);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_POSITION) == 0)
            {
                handle_position_command(s_command_str, current_cmd, start_index, &s_state, &s_game_data, s_current_fen, s_current_moves);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_GO) == 0)
            {
                handle_go_command(current_cmd, &s_state, &s_game_data, &s_search_result, s_debug);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_STOP) == 0)
            {
                handle_stop_command(&s_state, &s_search_result);
                break;
            }
            else if (strcmp(current_cmd, GE_CMD_PONDERHIT) == 0)
            {
                handle_ponderhit_command(&s_state);
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

static void reset_internal_data()
{
    uint64_t i;
    s_options = get_default_options();
    s_debug = false;
    s_state = get_default_state();
    for (i = 0; i <= COMMAND_STR_LEN; i++)
    {
        s_command_str[i] = '\0';
        s_edit_command_str[i] = '\0';
        s_current_moves[i] = '\0';
    }
    for (i = 0; i <= FEN_LENGTH_UPPER_BOUND; i++)
    {
        s_current_fen[i] = '\0';
    }
    reset_game_data(&s_game_data, START_FEN_STR);
    s_search_result = create_search_result();
}
