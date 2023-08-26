#include "bestmove_cmd_printer.h"
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_stop_command(engine_state *state, search_token *token)
{
    disable_debug_printing();
    if (is_working(*state))
    {
        cancel_search(token);
        on_cancelling_work(state);
        print_bestmove_response(&(token->result));
    }
}