#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include "position.h"

#define _POSITION_MASK 0x003F
#define _FLAGS_MASK 0x000F
#define _START_POS_BIT_INDEX 6
#define _FLAGS_BIT_INDEX 12
#define _CAPTURE_FLAGS_MASK 0x4
#define _PROMOTION_FLAGS_MASK 0x8
#define _CASTLING_FLAGS_PATTERN 0xA
#define _CASTLING_FLAGS_PATTERN_RESULT 0x2

typedef enum move_flags
{
    MOVE_QUIET = 0x0,
    MOVE_DOUBLE_PAWN_PUSH = 0x1,
    MOVE_KING_CASTLE = 0x2,
    MOVE_QUEEN_CASTLE = 0x3,
    MOVE_CAPTURE = 0x4,
    MOVE_EN_PASSANT_CAPTURE = 0x5,
    MOVE_KNIGHT_PROMOTION = 0x8,
    MOVE_BISHOP_PROMOTION = 0x9,
    MOVE_ROOK_PROMOTION = 0xA,
    MOVE_QUEEN_PROMOTION = 0xB,
    MOVE_KNIGHT_PROMOTION_CAPTURE = 0xC,
    MOVE_BISHOP_PROMOTION_CAPTURE = 0xD,
    MOVE_ROOK_PROMOTION_CAPTURE = 0xE,
    MOVE_QUEEN_PROMOTION_CAPTURE = 0xF
} move_flags;

typedef uint16_t move;

static inline bool is_movement_flags_valid(move_flags flags);
static inline bool is_movement_valid(move move);
static inline move create_movement(position start_pos, position dest_pos, move_flags flags);
static inline position get_start_pos(move move);
static inline position get_dest_pos(move move);
static inline move_flags get_flags(move move);
static inline bool is_capture(move_flags flags);
static inline bool is_en_passant(move_flags flags);
static inline bool is_promotion(move_flags flags);
static inline bool is_castling(move_flags flags);

static inline bool is_movement_flags_valid(move_flags flags)
{
    return (flags >= MOVE_QUIET && flags <= MOVE_EN_PASSANT_CAPTURE) || (flags >= MOVE_KNIGHT_PROMOTION && flags <= MOVE_QUEEN_PROMOTION_CAPTURE);
}

static inline bool is_movement_valid(move move)
{
    return is_position_valid(get_start_pos(move)) && is_position_valid(get_dest_pos(move)) && is_movement_flags_valid(get_flags(move));
}

static inline move create_movement(position start_pos, position dest_pos, move_flags flags)
{
    return (move)((((uint8_t)flags & _FLAGS_MASK) << _FLAGS_BIT_INDEX) | ((start_pos & _POSITION_MASK) << _START_POS_BIT_INDEX) | (dest_pos & _POSITION_MASK));
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