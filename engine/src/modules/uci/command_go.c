#include <string.h>
#include "../ai/time_manager.h"
#include "commands.h"
#include "consts.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

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

// Tokens are parsed in any order: values are collected first, then the time system is deduced
void handle_go_command(engine_state *state, board *board, search_token *search_token, bool debug)
{
    bool ponder, infinite;
    time_system time_system;
    go_cmd_edition edition;
    int64_t converted_value, wtime, btime, winc, binc, movestogo, movetime;
    uint64_t search_time;
    char *token_str, *convert_ptr;
    assert(state != NULL);
    assert(board != NULL);
    assert(search_token != NULL);
    if (!is_idling(*state))
    {
        return;
    }
    ponder = false;
    infinite = false;
    edition = GO_NONE;
    wtime = GO_OPT_NO_VALUE;
    btime = GO_OPT_NO_VALUE;
    winc = GO_OPT_NO_VALUE;
    binc = GO_OPT_NO_VALUE;
    movestogo = GO_OPT_NO_VALUE;
    movetime = GO_OPT_NO_VALUE;
    search_time = 0;
    token_str = strtok(NULL, UCI_DELIMITER);
    while (token_str != NULL)
    {
        if (strcmp(token_str, GE_CMD_GO_OPT_PONDER) == 0)
        {
            if (ponder)
            {
                return;
            }
            ponder = true;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_INFINITE) == 0)
        {
            if (infinite)
            {
                return;
            }
            infinite = true;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_WTIME) == 0)
        {
            if (edition != GO_NONE || wtime != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_WTIME;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_BTIME) == 0)
        {
            if (edition != GO_NONE || btime != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_BTIME;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_WINC) == 0)
        {
            if (edition != GO_NONE || winc != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_WINC;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_BINC) == 0)
        {
            if (edition != GO_NONE || binc != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_BINC;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_MOVESTOGO) == 0)
        {
            if (edition != GO_NONE || movestogo != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_MOVESTOGO;
        }
        else if (strcmp(token_str, GE_CMD_GO_OPT_MOVETIME) == 0)
        {
            if (edition != GO_NONE || movetime != GO_OPT_NO_VALUE)
            {
                return;
            }
            edition = GO_MOVETIME;
        }
        else if (edition == GO_NONE)
        {
            return;
        }
        else
        {
            converted_value = strtol(token_str, &convert_ptr, 10);
            if (convert_ptr == token_str)
            {
                return;
            }
            switch (edition)
            {
                case GO_WTIME:
                    if (converted_value <= 0)
                    {
                        return;
                    }
                    wtime = converted_value;
                    break;
                case GO_BTIME:
                    if (converted_value <= 0)
                    {
                        return;
                    }
                    btime = converted_value;
                    break;
                case GO_WINC:
                    if (converted_value < 0)
                    {
                        return;
                    }
                    winc = converted_value;
                    break;
                case GO_BINC:
                    if (converted_value < 0)
                    {
                        return;
                    }
                    binc = converted_value;
                    break;
                case GO_MOVESTOGO:
                    if (converted_value <= 0)
                    {
                        return;
                    }
                    movestogo = converted_value;
                    break;
                case GO_MOVETIME:
                    if (converted_value <= 0)
                    {
                        return;
                    }
                    movetime = converted_value;
                    break;
                default:
                    return;
            }
            edition = GO_NONE;
        }
        token_str = strtok(NULL, UCI_DELIMITER);
    }
    if (edition != GO_NONE)
    {
        return;
    }
    if (infinite)
    {
        if (wtime != GO_OPT_NO_VALUE || btime != GO_OPT_NO_VALUE || winc != GO_OPT_NO_VALUE || binc != GO_OPT_NO_VALUE
            || movestogo != GO_OPT_NO_VALUE || movetime != GO_OPT_NO_VALUE)
        {
            return;
        }
        time_system = TS_INFINITE;
    }
    else if (movetime != GO_OPT_NO_VALUE)
    {
        if (wtime != GO_OPT_NO_VALUE || btime != GO_OPT_NO_VALUE || winc != GO_OPT_NO_VALUE || binc != GO_OPT_NO_VALUE
            || movestogo != GO_OPT_NO_VALUE)
        {
            return;
        }
        time_system = TS_DEFINED;
        search_time = (uint64_t)movetime;
    }
    else if (movestogo != GO_OPT_NO_VALUE)
    {
        if (wtime == GO_OPT_NO_VALUE || btime == GO_OPT_NO_VALUE)
        {
            return;
        }
        time_system = TS_CONTROL;
        search_time = get_search_time_from_control(board, (uint64_t)wtime, (uint64_t)btime, (uint64_t)movestogo);
    }
    else if (winc != GO_OPT_NO_VALUE || binc != GO_OPT_NO_VALUE)
    {
        if (wtime == GO_OPT_NO_VALUE || btime == GO_OPT_NO_VALUE || winc == GO_OPT_NO_VALUE || binc == GO_OPT_NO_VALUE)
        {
            return;
        }
        time_system = TS_INCREMENTAL;
        search_time = get_search_time_from_incremental(board, (uint64_t)wtime, (uint64_t)winc, (uint64_t)btime, (uint64_t)binc);
    }
    else
    {
        return;
    }
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
