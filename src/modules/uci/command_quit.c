#include <stdlib.h>
#include "commands.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_quit_command(engine_state state, search_token *token)
{
    if (is_working(state))
    {
        cancel_search(token);
    }
    disable_debug_printing();
    exit(EXIT_SUCCESS);
}
