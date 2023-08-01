#ifndef POSITION_H
#define POSITION_H

#include "types.h"

static inline bool is_position_valid(position position);
static inline int8_t get_file(position position);
static inline vector2 to_position_vector(position position);

static inline bool is_position_valid(position position)
{
    return (position >= 0 && position < POSITIONS_COUNT) || position == NO_POSITION;
}

static inline int8_t get_file(position position)
{
    return position == NO_POSITION ? NO_FILE : position % RANKS_COUNT;
}

static inline vector2 to_position_vector(position position)
{
    vector2 vector;
    if (position == NO_POSITION) 
    {
        vector.x = NO_POSITION;
        vector.y = NO_POSITION;
    }
    else
    {
        vector.x = position % RANKS_COUNT;
        vector.y = position / RANKS_COUNT;
    }
    return vector;
}

#endif // POSITION_H