#include <string.h>
#include "../ai/time_manager.h"
#include "commands.h"
#include "consts.h"
#include "debug_printer.h"
#include "search_manager.h"
#include "engine_state.h"

#define GO_OPT_NO_VALUE -1

typedef enum go_cmd_edition
{
    GO_NONE,
    GO_WTIME,
    GO_BTIME,
    GO_WINC,
    GO_BINC,
    GO_MOVESTOGO,
    GO_MOVETIME
} go_cmd_edition;

void handle_go_command(char *edit_cmd, engine_state *state, board *board, search_token *search_token, bool debug)
{
    bool expecting_keyword, ponder;
    time_system time_system;
    go_cmd_edition edition;
    int64_t convertion_value, wtime, btime, winc, binc, movestogo, movetime;
    uint64_t search_time;
    char *convertion_ptr;
    assert(edit_cmd != NULL);
    assert(state != NULL);
    assert(board != NULL);
    assert(search_token != NULL);
    if (!is_idling(*state))
    {
        return;
    }
    expecting_keyword = true;
    ponder = false;
    time_system = TS_NONE;
    edition = GO_NONE;
    wtime = GO_OPT_NO_VALUE;
    btime = GO_OPT_NO_VALUE;
    winc = GO_OPT_NO_VALUE;
    binc = GO_OPT_NO_VALUE;
    movestogo = GO_OPT_NO_VALUE;
    movetime = GO_OPT_NO_VALUE;
    search_time = 0;
    edit_cmd = strtok(NULL, UCI_DELIMITER);
    while (edit_cmd != NULL)
    {
        if (strcmp(edit_cmd, GE_CMD_GO_OPT_PONDER) == 0)
        {
            if (!expecting_keyword)
            {
                return;
            }
            ponder = true;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_WTIME) == 0)
        {
            if (!expecting_keyword || time_system == TS_DEFINED || time_system == TS_INFINITE)
            {
                return;
            }
            expecting_keyword = false;
            edition = GO_WTIME;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_BTIME) == 0)
        {
            if (!expecting_keyword || time_system == TS_DEFINED || time_system == TS_INFINITE)
            {
                return;
            }
            expecting_keyword = false;
            edition = GO_BTIME;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_WINC) == 0)
        {
            if (!expecting_keyword || (time_system != TS_NONE && time_system != TS_INCREMENTAL))
            {
                return;
            }
            time_system = TS_INCREMENTAL;
            expecting_keyword = false;
            edition = GO_WINC;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_BINC) == 0)
        {
            if (!expecting_keyword || (time_system != TS_NONE && time_system != TS_INCREMENTAL))
            {
                return;
            }
            time_system = TS_INCREMENTAL;
            expecting_keyword = false;
            edition = GO_BINC;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_MOVESTOGO) == 0)
        {
            if (!expecting_keyword || time_system != TS_NONE)
            {
                return;
            }
            time_system = TS_CONTROL;
            expecting_keyword = false;
            edition = GO_MOVESTOGO;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_MOVETIME) == 0)
        {
            if (!expecting_keyword || time_system != TS_NONE)
            {
                return;
            }
            time_system = TS_DEFINED;
            expecting_keyword = false;
            edition = GO_MOVETIME;
        }
        else if (strcmp(edit_cmd, GE_CMD_GO_OPT_INFINITE) == 0)
        {
            if (!expecting_keyword || time_system != TS_NONE)
            {
                return;
            }
            time_system = TS_INFINITE;
        }
        else if (expecting_keyword)
        {
            return;
        }
        else
        {
            convertion_value = strtol(edit_cmd, &convertion_ptr, 10);
            if (convertion_ptr == edit_cmd)
            {
                return;
            }
            switch (edition)
            {
                case GO_WTIME:
                    if (convertion_value <= 0)
                    {
                        return;
                    }
                    wtime = convertion_value;
                    break;
                case GO_BTIME:
                    if (convertion_value <= 0)
                    {
                        return;
                    }
                    btime = convertion_value;
                    break;
                case GO_WINC:
                    if (convertion_value < 0)
                    {
                        return;
                    }
                    winc = convertion_value;
                    break;
                case GO_BINC:
                    if (convertion_value < 0)
                    {
                        return;
                    }
                    binc = convertion_value;
                    break;
                case GO_MOVESTOGO:
                    if (convertion_value <= 0)
                    {
                        return;
                    }
                    movestogo = convertion_value;
                    break;
                case GO_MOVETIME:
                    if (convertion_value <= 0)
                    {
                        return;
                    }
                    movetime = convertion_value;
                    break;
                default:
                    return;
            }
            edition = GO_NONE;
            expecting_keyword = true;
        }
        edit_cmd = strtok(NULL, UCI_DELIMITER);
    }
    switch (time_system)
    {
        case TS_INFINITE:
            break;
        case TS_DEFINED:
            if (movetime != GO_OPT_NO_VALUE)
            {
                search_time = (uint64_t)movetime;
            }
            break;
        case TS_INCREMENTAL:
            if (wtime != GO_OPT_NO_VALUE && btime != GO_OPT_NO_VALUE && winc != GO_OPT_NO_VALUE && binc != GO_OPT_NO_VALUE)
            {
                search_time = get_search_time_from_incremental(board, (uint64_t)wtime, (uint64_t)winc, (uint64_t)btime, (uint64_t)binc);
            }
            break;
        case TS_CONTROL:
            if (wtime != GO_OPT_NO_VALUE && btime != GO_OPT_NO_VALUE && movestogo != GO_OPT_NO_VALUE)
            {
                search_time = get_search_time_from_control(board, (uint64_t)wtime, (uint64_t)btime, (uint64_t)movestogo);
            }
            break;
        default:
            return;
    }
    if (time_system == TS_INFINITE || search_time != 0)
    {
        on_starting_work(state, ponder);
        if (debug)
        {
            enable_debug_printing(&(search_token->result));
        }
        reset_token(search_token);
        if (time_system == TS_INFINITE)
        {
            start_search_infinite(search_token, board, ponder);
        }
        else
        {
            start_search_timed(search_token, board, ponder, search_time);
        }
    }
}
