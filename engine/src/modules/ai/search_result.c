#include "../core/enhanced_time.h"
#include "../core/move.h"
#include "search_result.h"

// The caller must hold the engine mutex
search_result create_search_result()
{
    search_result result;
    reset_search_result(&result, false);
    return result;
}

// The caller must hold the engine mutex
void reset_search_result(search_result *result, bool start_now)
{
    assert(result != NULL);
    result->valid = false;
    result->finished = false;
    result->is_mate = false;
    result->centipawns_score = 0.0;
    result->mate_score = 0;
    result->best_move = create_empty_movement();
    result->ponder_move = create_empty_movement();
    result->depth = 0;
    result->nodes_explored = 0;
    result->start_time = start_now ? get_current_uptime() : 0;
}
