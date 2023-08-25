#include "../ai/engine.h"
#include "bestmove_cmd_printer.h"
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"

void handle_stop_command(engine_state *state, search_result *result)
{
    disable_debug_printing();
    if (is_working(*state))
    {
        cancel_search();
        on_cancelling_work(state);
        print_bestmove_response(result);
    }
}