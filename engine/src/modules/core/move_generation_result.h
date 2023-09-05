#ifndef MOVE_GENERATION_RESULT_H
#define MOVE_GENERATION_RESULT_H

#include "types.h"

static inline move_generation_result create_move_generation_result();
static inline void reset_move_generation_result(move_generation_result *result);

static inline move_generation_result create_move_generation_result()
{
    move_generation_result result;
    result.moves_count = 0;
    result.is_currently_check = false;
    return result;
}

static inline void reset_move_generation_result(move_generation_result *result)
{
    assert(result != NULL);
    result->moves_count = 0;
    result->is_currently_check = false;
}

#endif // MOVE_GENERATION_RESULT_H
