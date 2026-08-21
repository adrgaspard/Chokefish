#include "search_utils.h"

void publish_search_result(search_context *context, uint16_t depth, evaluation *evaluation)
{
    search_result *result;
    assert(context != NULL);
    assert(evaluation != NULL);
    result = context->result;
    engine_mutex_lock(context->mutex);
    result->depth = depth;
    engine_atomic_uint64_store(&(result->nodes_explored), context->nodes_explored);
    result->best_move = evaluation->best_move;
    result->ponder_move = evaluation->ponder_move;
    if (IS_END_SCORE(evaluation->score))
    {
        result->is_mate = true;
        result->mate_score = (int16_t)((evaluation->score < DRAW_SCORE) ? LOSE_SCORE - evaluation->score : WIN_SCORE - evaluation->score);
    }
    else
    {
        result->is_mate = false;
        result->centipawns_score = evaluation->score / 100.0;
    }
    result->valid = true;
    engine_mutex_unlock(context->mutex);
}