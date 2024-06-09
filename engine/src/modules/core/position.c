#include <assert.h>
#include "position.h"

bool is_position_valid(position position)
{
    return (position >= 0 && position < POSITIONS_COUNT) || position == NO_POSITION;
}

int8_t get_file(position position)
{
    assert(is_position_valid(position));
    return position == NO_POSITION ? NO_FILE : position % RANKS_COUNT;
}

int8_t get_rank(position position)
{
    assert(is_position_valid(position));
    return position == NO_POSITION ? NO_RANK : position / RANKS_COUNT;
}

vector2 to_position_vector(position position)
{
    vector2 vector;
    assert(is_position_valid(position));
    if (position == NO_POSITION) 
    {
        vector.x = NO_FILE;
        vector.y = NO_RANK;
    }
    else
    {
        vector.x = position % RANKS_COUNT;
        vector.y = position / RANKS_COUNT;
    }
    return vector;
}

index_validation_result compute_index_if_valid(int32_t x, int32_t y)
{
    index_validation_result result;
    result.valid = x >= 0 && x < FILES_COUNT && y >= 0 && y < RANKS_COUNT;
    result.index = result.valid ? (position)(y * FILES_COUNT + x) : NO_POSITION;
    return result;
}
