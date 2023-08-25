#include "commands.h"
#include "engine_state.h"

void handle_ponderhit_command(engine_state *state)
{
    if (is_pondering(*state))
    {
        on_switching_from_ponder_to_search(state);
    }
}