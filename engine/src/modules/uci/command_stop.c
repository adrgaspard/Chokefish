#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_stop_command(engine_state *state, search_token *token)
{
    assert(state != NULL);
    assert(token != NULL);
    disable_debug_printing(&(token->result), false);
    if (is_working(*state))
    {
        cancel_search(token, false);
        on_cancelling_work(state);
    }
}