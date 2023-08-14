#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "uci.h"
#include "consts.h"

#define COMMAND_STR_LEN 20000
#define COMMAND_STR_LEN_WITH_ENDCHAR (COMMAND_STR_LEN + 1)

static char s_command_str[COMMAND_STR_LEN_WITH_ENDCHAR] = "";

void handle_commands()
{
    bool uci;
    char *current_arg;
    while (true)
    {
        if (fgets(s_command_str, COMMAND_STR_LEN, stdin) == NULL)
        {
            break;
        }
        current_arg = strtok(s_command_str, UCI_DELIMITER);
    }
}