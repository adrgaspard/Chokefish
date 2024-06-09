#ifndef MOVE_DATA_H
#define MOVE_DATA_H

#include "types.h"

move_data create_move_data(position start_pos, position dest_pos, promotion_type promotion_type);
move_data create_empty_move_data();

#endif // MOVE_DATA_H
