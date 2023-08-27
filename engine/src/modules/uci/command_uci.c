#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "engine_options.h"
#include "engine_state.h"

void handle_uci_command(engine_state *state, engine_options options)
{
    if (is_waiting_for_setup(*state))
    {
        // Identifying engine
        printf(EG_CMD_ID " " EG_CMD_ID_OPT_NAME " " ENGINE_NAME "\n");
        printf(EG_CMD_ID " " EG_CMD_ID_OPT_AUTHOR " " ENGINE_AUTHOR "\n");

        // Sending options
        print_options(options);

        // Actualize state
        on_sending_uciok(state);
        printf(EG_CMD_UCIOK "\n");
    }
}
