#include <stdlib.h>
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_quit_command(engine_state state, search_token *token)
{
    assert(token != NULL);
    if (is_working(state))
    {
        cancel_search(token, true);
    }
    disable_debug_printing(&(token->result), false);
    exit(EXIT_SUCCESS);
}
