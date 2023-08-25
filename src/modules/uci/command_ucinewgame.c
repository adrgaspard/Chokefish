#include "../ai/engine.h"
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"

void handle_ucinewgame_command(engine_state *state)
{
    if (!is_waiting_for_setup(*state) && !is_waiting_for_ready(*state))
    {
        if (is_working(*state))
        {
            on_cancelling_work(state);
            cancel_search();
        }
        disable_debug_printing();
        reset_engine_cache();
    }
}
