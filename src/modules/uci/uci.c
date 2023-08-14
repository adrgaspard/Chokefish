#include <stdio.h>
#include <string.h>
#include "uci.h"
#include "engine_options.h"
#include "engine_state.h"

#define COMMAND_STR_LEN 20000
#define COMMAND_STR_LEN_WITH_ENDCHAR (COMMAND_STR_LEN + 1)

static char s_command_str[COMMAND_STR_LEN_WITH_ENDCHAR] = "";
static engine_options s_options;
static engine_state s_state;
static bool s_debug;

static void handle_uci_command();
static void handle_debug_command(char *current_cmd);
static void handle_isready_command();
static void handle_ucinewgame_command();
static void handle_quit_command();

void handle_commands()
{
    char *current_cmd;
    s_state = get_default_state();
    s_options = get_default_options();
    s_debug = false;
    while (true)
    {
        if (fgets(s_command_str, COMMAND_STR_LEN, stdin) == NULL)
        {
            break;
        }
        current_cmd = strtok(s_command_str, UCI_DELIMITER);
        while (current_cmd != NULL)
        {
            if (strcmp(current_cmd, GE_CMD_UCI) == 0)
            {
                handle_uci_command();
            }
            else if (strcmp(current_cmd, GE_CMD_DEBUG) == 0)
            {
                handle_debug_command(current_cmd);
            }
            else if (strcmp(current_cmd, GE_CMD_ISREADY) == 0)
            {
                handle_isready_command();
            }
            else if (strcmp(current_cmd, GE_CMD_SETOPTION) == 0)
            {

            }
            else if (strcmp(current_cmd, GE_CMD_UCINEWGAME) == 0)
            {
                handle_ucinewgame_command();
            }
            else if (strcmp(current_cmd, GE_CMD_POSITION) == 0)
            {

            }
            else if (strcmp(current_cmd, GE_CMD_GO) == 0)
            {

            }
            else if (strcmp(current_cmd, GE_CMD_STOP) == 0)
            {

            }
            else if (strcmp(current_cmd, GE_CMD_PONDERHIT) == 0)
            {

            }
            else if (strcmp(current_cmd, GE_CMD_QUIT) == 0)
            {
                handle_quit_command();
            }
            else
            {
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

static void handle_ucinewgame_command()
{
    if (!is_waiting_for_setup(s_state))
    {
        // TODO : if searching: stop the search, don't return anything
        // TODO : clear all search cache, put the state on idling
    }
}

static void handle_quit_command()
{
    exit(EXIT_SUCCESS);
}
