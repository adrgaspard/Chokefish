#ifndef MOVE_DATA_H
#define MOVE_DATA_H

#include "types.h"

static inline move_data create_move_data(position start_pos, position dest_pos, promotion_type promotion_type);
static inline move_data create_empty_move_data();

static inline move_data create_move_data(position start_pos, position dest_pos, promotion_type promotion_type)
{
    move_data data;
    data.start_pos = start_pos;
    data.dest_pos = dest_pos;
    data.promotion_type = promotion_type;
    return data;
}

static inline move_data create_empty_move_data()
{
    return create_move_data(NO_POSITION, NO_POSITION, PROMOTION_NONE);
}

#endif // MOVE_DATA_H
