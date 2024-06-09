#include <pthread.h>
#include "../core/enhanced_time.h"
#include "../core/move.h"
#include "search_result.h"

search_result create_search_result()
{
    search_result result;
    pthread_rwlock_init(&(result.lock), NULL);
    reset_search_result(&result, false);
    return result;
}

void reset_search_result(search_result *result, bool start_now)
{
    assert(result != NULL);
    pthread_rwlock_wrlock(&(result->lock));
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
    pthread_rwlock_unlock(&(result->lock));
}