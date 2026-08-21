#ifndef SEARCH_UTILS_H
#define SEARCH_UTILS_H

#include "../core/threading.h"
#include "types.h"

#define MAX_DEPTH 500
#define DRAW_SCORE 0
#define WIN_SCORE 1000000000
#define LOSE_SCORE -(WIN_SCORE)
#define IS_END_SCORE(score) (score > (WIN_SCORE - 10000) || score < (LOSE_SCORE + 10000))

typedef struct search_context
{
    search_result *result;
    engine_mutex *mutex;
    engine_atomic_bool *cancellation_requested;
    uint64_t nodes_explored;
} search_context;

typedef struct evaluation
{
    int32_t score;
    move best_move;
    move ponder_move;
} evaluation;

void publish_search_result(search_context *context, uint16_t depth, evaluation *evaluation);

#endif // SEARCH_UTILS_H