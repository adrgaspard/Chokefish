#include "../ai/search_result.h"
#include "engine_options.h"
#include "engine_state.h"
#include "uci_data.h"

move_generation_options g_all_moves;
char g_command_str[COMMAND_STR_LEN + 1];
char g_current_fen[FEN_LENGTH_UPPER_BOUND + 1];
char g_current_moves[COMMAND_STR_LEN + 1];
bool g_debug;
pthread_t g_debug_thread_id;
engine_options g_options;
engine_state g_state;
search_result g_search_result;

void initialize_or_reset_uci_data()
{
    uint64_t i;
    g_all_moves.include_quiet_moves = true;
    g_all_moves.promotion_types_to_include = PROMOTION_ALL;
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