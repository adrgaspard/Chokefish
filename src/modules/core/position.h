#ifndef POSITION_H
#define POSITION_H

#include "types.h"

static inline bool is_position_valid(position position);

static inline bool is_position_valid(position position)
{
    return position < POSITIONS_COUNT || position == NO_POSITION;
}

#endif // POSITION_H