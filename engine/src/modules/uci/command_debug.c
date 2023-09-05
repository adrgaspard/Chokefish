#include <string.h>
#include "commands.h"
#include "consts.h"
#include "debug_printer.h"
#include "engine_state.h"

void handle_debug_command(char *edit_cmd, bool *debug, engine_state state, search_result *result)
{
    assert(edit_cmd != NULL);
    assert(debug != NULL);
    assert(result != NULL);
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd != NULL)
    {
        if (strcmp(edit_cmd, GE_CMD_DEBUG_OPT_ON) == 0)
        {
            *debug = true;
            if (is_working(state))
            {
                enable_debug_printing(result);
            }
        }
        else if (strcmp(edit_cmd, GE_CMD_DEBUG_OPT_OFF) == 0)
        {
            *debug = false;
            disable_debug_printing(result, false);
        }
    }
}
