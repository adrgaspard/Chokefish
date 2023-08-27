#include "../ai/engine.h"
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_ucinewgame_command(engine_state *state, search_token *token)
{
    if (!is_waiting_for_setup(*state) && !is_waiting_for_ready(*state))
    {
        if (is_working(*state))
        {
            on_cancelling_work(state);
            cancel_search(token, true);
        }
        disable_debug_printing(&(token->result), false);
        reset_engine_cache();
    }
}
