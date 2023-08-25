#include <stdlib.h>
#include "../ai/engine.h"
#include "commands.h"
#include "debug_printer.h"

void handle_quit_command()
{
    cancel_search();
    disable_debug_printing();
    exit(EXIT_SUCCESS);
}
