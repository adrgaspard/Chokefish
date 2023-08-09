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

static inline bool is_movement_flags_valid(move_flags flags);
static inline bool is_movement_valid(move move);
static inline move create_movement(position start_pos, position dest_pos, move_flags flags);
static inline move create_empty_movement();
static inline position get_start_pos(move move);
static inline position get_dest_pos(move move);
static inline move_flags get_flags(move move);
static inline bool is_capture(move_flags flags);
static inline bool is_en_passant(move_flags flags);
static inline bool is_promotion(move_flags flags);
static inline bool is_castling(move_flags flags);
static inline piece_type get_promotion_piece_type(move_flags flags);

static inline bool is_movement_flags_valid(move_flags flags)
{
    return (flags >= MOVE_QUIET && flags <= MOVE_EN_PASSANT_CAPTURE) || (flags >= MOVE_KNIGHT_PROMOTION && flags <= MOVE_QUEEN_PROMOTION_CAPTURE);
}

static inline bool is_movement_valid(move move)
{
    return (is_position_valid(get_start_pos(move)) && is_position_valid(get_dest_pos(move)) && is_movement_flags_valid(get_flags(move)))
        || move == create_empty_movement();
}

static inline move create_movement(position start_pos, position dest_pos, move_flags flags)
{
    return (move)((((uint8_t)flags & _FLAGS_MASK) << _FLAGS_BIT_INDEX) | ((start_pos & _POSITION_MASK) << _START_POS_BIT_INDEX) | (dest_pos & _POSITION_MASK));
}

static inline move create_empty_movement()
{
    return create_movement(NO_POSITION, NO_POSITION, MOVE_QUIET);
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

static inline piece_type get_promotion_piece_type(move_flags flags)
{
    return is_promotion(flags) ? (PIECE_KNIGHT + (piece_type)(flags & _SPECIALS_FLAGS_MASK)) : PIECE_NONE;
}

#endif // MOVE_H