#ifndef BITBOARD_H
#define BITBOARD_H

#include "precomputed_board_data.h"
#include "types.h"

static inline void set_position_to_zero(bitboard *bitboard, position position);
static inline void set_position_to_one(bitboard *bitboard, position position);
static inline void toggle_position(bitboard *bitboard, position position);
static inline void toggle_positions(bitboard *bitboard, position first, position second);
static inline bool contains_position(bitboard bitboard, position position);
static inline int8_t pop_least_significant_bit(bitboard *bitboard);
static inline bitboard shift(bitboard bitboard, int8_t shift_quantity);
static inline bitboard get_pawn_attacks_mask(bitboard pawn_bitboard, color color);

static inline void set_position_to_zero(bitboard *bitboard, position position)
{
    assert(bitboard != NULL);
    *bitboard &= ~(1ULL << position);
}

static inline void set_position_to_one(bitboard *bitboard, position position)
{
    assert(bitboard != NULL);
    *bitboard |= (1ULL << position);
}

static inline void toggle_position(bitboard *bitboard, position position)
{
    assert(bitboard != NULL);
    *bitboard ^= (1ULL << position);
}

static inline void toggle_positions(bitboard *bitboard, position first, position second)
{
    assert(bitboard != NULL);
    *bitboard ^= (1ULL << first | 1ULL << second);
}

static inline bool contains_position(bitboard bitboard, position position)
{
    return ((bitboard >> position) & 1) != 0;
}

static inline int8_t pop_least_significant_bit(bitboard *bitboard)
{
    int8_t count;
    assert(bitboard != NULL);
    count = (int8_t)__builtin_ctzll(*bitboard);
    *bitboard = *bitboard & (*bitboard - 1);
    return count;
}

static inline bitboard shift(bitboard bitboard, int8_t shift_quantity)
{
    return shift_quantity >= 0 ? bitboard << shift_quantity : bitboard >> -shift_quantity;
}

static inline bitboard get_pawn_attacks_mask(bitboard pawn_bitboard, color color)
{
    return color == COLOR_WHITE
        ? ((pawn_bitboard << (FILES_COUNT + 1)) & g_not_file_mask[0]) | ((pawn_bitboard << (FILES_COUNT - 1)) & g_not_file_mask[FILES_COUNT - 1])
        : ((pawn_bitboard >> (FILES_COUNT - 1)) & g_not_file_mask[0]) | ((pawn_bitboard >> (FILES_COUNT + 1)) & g_not_file_mask[FILES_COUNT - 1]);
}


#endif // BITBOARD_H
