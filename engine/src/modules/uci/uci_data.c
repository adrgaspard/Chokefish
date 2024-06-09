#include "../ai/search_result.h"
#include "engine_options.h"
#include "engine_state.h"
#include "uci_data.h"

void initialize_or_reset_uci_data()
{
    uint64_t i;
    for (i = 0; i <= COMMAND_STR_LEN; i++)
    {
        g_command_str[i] = '\0';
        g_current_moves[i] = '\0';
    }
    for (i = 0; i <= FEN_LENGTH_UPPER_BOUND; i++)
    {
        g_current_fen[0] = '\0';
    }

    g_state = get_default_state();
    g_options = get_default_options();
    g_debug = false;
    g_search_result = create_search_result();
}