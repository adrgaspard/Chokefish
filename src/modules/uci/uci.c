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

static void handle_uci_command();

void handle_commands()
{
    char *current_arg;
    s_state = get_default_state();
    s_options = get_default_options();
    while (true)
    {
        if (fgets(s_command_str, COMMAND_STR_LEN, stdin) == NULL)
        {
            break;
        }
        current_arg = strtok(s_command_str, UCI_DELIMITER);
        while (current_arg != NULL)
        {
            if (strcmp(current_arg, GE_CMD_UCI) == 0)
            {
                handle_uci_command();
            }
            else if (strcmp(current_arg, GE_CMD_DEBUG) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_ISREADY) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_SETOPTION) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_UCINEWGAME) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_POSITION) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_GO) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_STOP) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_PONDERHIT) == 0)
            {

            }
            else if (strcmp(current_arg, GE_CMD_QUIT) == 0)
            {

            }
            else
            {
                current_arg = strtok(NULL, UCI_DELIMITER);
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