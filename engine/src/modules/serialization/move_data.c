#include <assert.h>
#include "move_data.h"
#include "position.h"

move_data create_move_data(position start_pos, position dest_pos, promotion_type promotion_type)
{
    move_data data;
    assert(is_position_valid(start_pos));
    assert(is_position_valid(dest_pos));
    data.start_pos = start_pos;
    data.dest_pos = dest_pos;
    data.promotion_type = promotion_type;
    return data;
}

move_data create_empty_move_data()
{
    return create_move_data(NO_POSITION, NO_POSITION, PROMOTION_NONE);
}
