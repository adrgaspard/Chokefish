#ifndef AI_TYPES_H
#define AI_TYPES_H

#include <pthread.h>
#include "../core/types.h"

typedef struct search_result
{
    pthread_rwlock_t lock;
    bool valid;
    bool finished;
    bool is_mate;
    double centipawns_score;
    uint16_t mate_score;
    move best_move;
    move ponder_move;
    uint16_t depth;
    uint64_t nodes_explored;
    uint64_t start_time;
} search_result;

#endif // AI_TYPES_H
