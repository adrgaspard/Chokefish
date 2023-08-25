#include <string.h>
#include "../ai/engine.h"
#include "commands.h"
#include "consts.h"
#include "debug_printer.h"
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

static void do_search_preprocessing(engine_state *state, search_result *result, bool ponder, bool debug);

void handle_go_command(char *edit_cmd, engine_state *state, game_data *game_data, search_result *search_result, bool debug)
{
    bool expecting_keyword, ponder;
    time_system time_system;
    go_cmd_edition edition;
    int64_t convertion_value, wtime, btime, winc, binc, movestogo, movetime;
    char *convertion_ptr;
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
            time_system = TS_CONTROL;
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
            do_search_preprocessing(state, search_result, ponder, debug);
            start_search_infinite_time(game_data, search_result);
            return;
        case TS_DEFINED:
            if (movetime != GO_OPT_NO_VALUE)
            {
                do_search_preprocessing(state, search_result, ponder, debug);
                start_search_defined_time(game_data, search_result, (uint64_t)movetime);
            }
            return;
        case TS_INCREMENTAL:
            if (wtime != GO_OPT_NO_VALUE && btime != GO_OPT_NO_VALUE && winc != GO_OPT_NO_VALUE && binc != GO_OPT_NO_VALUE)
            {
                do_search_preprocessing(state, search_result, ponder, debug);
                start_search_incremental_time(game_data, search_result, (uint64_t)wtime, (uint64_t)winc, (uint64_t)btime, (uint64_t)binc);
            }
            return;
        case TS_CONTROL:
            if (wtime != GO_OPT_NO_VALUE && btime != GO_OPT_NO_VALUE && movestogo != GO_OPT_NO_VALUE)
            {
                do_search_preprocessing(state, search_result, ponder, debug);
                start_search_time_control(game_data, search_result, (uint64_t)wtime, (uint64_t)btime, (uint64_t)movestogo);
            }
            return;
        default:
            return;
    }
}

static void do_search_preprocessing(engine_state *state, search_result *result, bool ponder, bool debug)
{
    on_starting_work(state, ponder);
    if (debug)
    {
        enable_debug_printing(result);
    }
}
