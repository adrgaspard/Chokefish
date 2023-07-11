#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include "position.h"

#define _POSITION_MASK 0x0000000000111111
#define _FLAGS_MASK 0x0000000000001111
#define _START_POS_BIT_INDEX 6
#define _FLAGS_BIT_INDEX 12
#define _CAPTURE_FLAGS_MASK 0b0100
#define _PROMOTION_FLAGS_MASK 0b1000
#define _CASTLING_FLAGS_PATTERN 0b1010
#define _CASTLING_FLAGS_PATTERN_RESULT 0b0010

typedef enum move_flags
{
    MOVE_QUIET = 0b0000,
    MOVE_DOUBLE_PAWN_PUSH = 0b0001,
    MOVE_KING_CASTLE = 0b0010,
    MOVE_QUEEN_CASTLE = 0b0011,
    MOVE_CAPTURE = 0b0100,
    MOVE_EN_PASSANT_CAPTURE = 0b0101,
    MOVE_KNIGHT_PROMOTION = 0b1000,
    MOVE_BISHOP_PROMOTION = 0b1001,
    MOVE_ROOK_PROMOTION = 0b1010,
    MOVE_QUEEN_PROMOTION = 0b1011,
    MOVE_KNIGHT_PROMOTION_CAPTURE = 0b1100,
    MOVE_BISHOP_PROMOTION_CAPTURE = 0b1101,
    MOVE_ROOK_PROMOTION_CAPTURE = 0b1110,
    MOVE_QUEEN_PROMOTION_CAPTURE = 0b1111
} move_flags;

typedef uint16_t move;

static inline bool is_movement_flags_valid(move_flags flags)
{
    return (flags >= MOVE_QUIET && flags <= MOVE_EN_PASSANT_CAPTURE) || (flags >= MOVE_KNIGHT_PROMOTION && flags <= MOVE_QUEEN_PROMOTION_CAPTURE);
}

static inline move create_movement(position start_pos, position dest_pos, move_flags flags)
{
    return ((flags & _FLAGS_MASK) << _FLAGS_BIT_INDEX) | ((start_pos & _POSITION_MASK) << _START_POS_BIT_INDEX) | (dest_pos & _POSITION_MASK);
}

static inline position get_start_pos(move move)
{
    return (move >> _START_POS_BIT_INDEX) & _POSITION_MASK;
}

static inline position get_dest_pos(move move)
{
    return move & _POSITION_MASK;
}

static inline move_flags get_flags(move move)
{
    return (move >> _FLAGS_BIT_INDEX) & _FLAGS_MASK;
}

static inline bool is_movement_valid(move move)
{
    return is_position_valid(get_start_pos(move)) && is_position_valid(get_dest_pos(move)) && is_movement_flags_valid(get_flags(move));
}

static inline bool is_capture(move_flags flags)
{
    return (flags & _CAPTURE_FLAGS_MASK) != 0;
}

static inline bool is_en_passant(move_flags flags)
{
    return flags == MOVE_EN_PASSANT_CAPTURE;
}

static inline bool is_promotion(move_flags flags) 
{
    return (flags & _PROMOTION_FLAGS_MASK) != 0;
}

static inline bool is_castling(move_flags flags)
{
    return (flags & _CASTLING_FLAGS_PATTERN) == _CASTLING_FLAGS_PATTERN_RESULT;
}

#endif // MOVE_H