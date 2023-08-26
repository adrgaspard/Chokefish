#include <string.h>
#include "commands.h"
#include "engine_options.h"

void handle_setoption_command(char *edit_cmd, engine_options *options)
{
    char name[OPT_NAME_MAX_LENGTH + 1], value[OPT_VALUE_MAX_LENGHT + 1];
    uint64_t value_len;
    value[0] = '\0';
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd == NULL || strcmp(edit_cmd, GE_CMD_SETOPTION_OPT_NAME) != 0)
    {
        return;
    }
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd == NULL || strlen(edit_cmd) > OPT_NAME_MAX_LENGTH)
    {
        return;
    }
    else
    {
        strcpy(name, edit_cmd);
    }
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd == NULL || strcmp(edit_cmd, GE_CMD_SETOPTION_OPT_VALUE) != 0)
    {
        return;
    }
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    if (edit_cmd == NULL)
    {
        return;
    }
    value_len = 0;
    while (edit_cmd != NULL)
    {
        value_len += strlen(edit_cmd);
        if (value_len > OPT_VALUE_MAX_LENGHT)
        {
            return;
        }
        strcat(value, edit_cmd);
        edit_cmd = strtok(NULL, UCI_DELIMITER);
        if (edit_cmd != NULL)
        {
            value_len += strlen(UCI_DELIMITER);
            if (value_len > OPT_VALUE_MAX_LENGHT)
            {
                return;
            }
            strcat(value, UCI_DELIMITER);
        }
    }
    setoption(options, name, value);
}
