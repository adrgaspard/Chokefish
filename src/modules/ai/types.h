#ifndef AI_TYPES_H
#define AI_TYPES_H

#include "../core/types.h"

typedef struct search_result
{
    bool valid;
    bool finished;
    move best_move;
} search_result;

#endif // AI_TYPES_H
