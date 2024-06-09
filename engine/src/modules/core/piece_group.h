#ifndef PIECE_GROUP_H
#define PIECE_GROUP_H

#include "types.h"

bool is_piece_group_valid(piece_group *group);
piece_group create_piece_group();
void add_position_to_group(piece_group *group, position position);
void remove_position_from_group(piece_group *group, position position);
void move_position_in_group(piece_group *group, position start_pos, position dest_pos);

#endif // PIECE_GROUP_H
