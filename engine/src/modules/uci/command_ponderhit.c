#include "../ai/time_manager.h"
#include "../core/enhanced_time.h"
#include "commands.h"
#include "engine_state.h"
#include "search_manager.h"

void handle_ponderhit_command(engine_state *state, search_token *token)
{
    uint64_t new_search_time;
    assert(state != NULL);
    assert(token != NULL);
    if (is_pondering(*state))
    {
        on_switching_from_ponder_to_search(state);
        if (token->infinite_time)
        {
            stop_pondering_infinite(token);
        }
        else
        {
            new_search_time = get_search_time_after_pondering(token->search_time, get_current_uptime() - token->ponder_start_time);
            stop_pondering_timed(token, new_search_time);
        }
    }
}