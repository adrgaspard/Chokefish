#ifndef POSITION_H
#define POSITION_H

#include "types.h"

static inline bool is_position_valid(position position);
static inline int8_t get_file(position position);

static inline bool is_position_valid(position position)
{
    return (position >= 0 && position < POSITIONS_COUNT) || position == NO_POSITION;
}

static inline int8_t get_file(position position)
{
    return position == NO_POSITION ? NO_FILE : position % RANKS_COUNT;
}

#endif // POSITION_H