#ifndef MOVE_H
#define MOVE_H

#include "types.h"

bool is_move_flags_valid(move_flags flags);
bool is_move_valid(move move);
bool is_move_empty(move move);
move create_move(position start_pos, position dest_pos, move_flags flags);
move create_empty_move();
position get_start_pos(move move);
position get_dest_pos(move move);
move_flags get_flags(move move);
bool is_capture(move_flags flags);
bool is_en_passant(move_flags flags);
bool is_promotion(move_flags flags);
bool is_castling(move_flags flags);
piece_type get_promotion_piece_type(move_flags flags);

#endif // MOVE_H