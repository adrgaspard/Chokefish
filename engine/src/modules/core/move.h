#ifndef MOVE_H
#define MOVE_H

#include "types.h"
#include "position.h"

#define _POSITION_MASK 0x003F
#define _FLAGS_MASK 0x000F
#define _START_POS_BIT_INDEX 6
#define _FLAGS_BIT_INDEX 12
#define _CAPTURE_FLAGS_MASK 0x4
#define _PROMOTION_FLAGS_MASK 0x8
#define _SPECIALS_FLAGS_MASK 0x3
#define _CASTLING_FLAGS_PATTERN 0xA
#define _CASTLING_FLAGS_PATTERN_RESULT 0x2
#define _GET_START_POS(move) ((move >> _START_POS_BIT_INDEX) & _POSITION_MASK)
#define _GET_DEST_POS(move) (move & _POSITION_MASK)
#define _GET_FLAGS(move) ((move >> _FLAGS_BIT_INDEX) & _FLAGS_MASK)

bool is_movement_flags_valid(move_flags flags);
bool is_movement_valid(move move);
bool is_movement_empty(move move);
move create_movement(position start_pos, position dest_pos, move_flags flags);
move create_empty_movement();
position get_start_pos(move move);
position get_dest_pos(move move);
move_flags get_flags(move move);
bool is_capture(move_flags flags);
bool is_en_passant(move_flags flags);
bool is_promotion(move_flags flags);
bool is_castling(move_flags flags);
piece_type get_promotion_piece_type(move_flags flags);

#endif // MOVE_H
