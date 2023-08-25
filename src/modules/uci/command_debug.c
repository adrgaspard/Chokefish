#include <string.h>
#include "commands.h"
#include "consts.h"

void handle_debug_command(char *edit_cmd, bool *debug)
{  
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd != NULL)
    {
        if (strcmp(edit_cmd, GE_CMD_DEBUG_OPT_ON) == 0)
        {
            *debug = true;
        }
        else if (strcmp(edit_cmd, GE_CMD_DEBUG_OPT_OFF) == 0)
        {
            *debug = false;
        }
    }
}
