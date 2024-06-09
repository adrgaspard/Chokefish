#include <assert.h>
#include "move_generation_result.h"

move_generation_result create_move_generation_result()
{
    move_generation_result result;
    result.moves_count = 0;
    result.is_currently_check = false;
    return result;
}

void reset_move_generation_result(move_generation_result *result)
{
    assert(result != NULL);
    result->moves_count = 0;
    result->is_currently_check = false;
}
