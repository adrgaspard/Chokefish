#include <stdlib.h>
#include "../ai/engine.h"
#include "commands.h"

void handle_quit_command()
{
    cancel_search();
    exit(EXIT_SUCCESS);
}
