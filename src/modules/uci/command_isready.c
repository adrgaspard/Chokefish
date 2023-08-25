#include <stdio.h>
#include "commands.h"
#include "consts.h"
#include "engine_state.h"

void handle_isready_command(engine_state *state)
{
    if (!is_waiting_for_setup(*state))
    {
        on_sending_readyok(state);
        printf(EG_CMD_READYOK "\n");
    }
}
