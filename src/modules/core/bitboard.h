#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

static inline void set_position_to_zero(bitboard bitboard, position position);
static inline void set_position_to_one(bitboard bitboard, position position);
static inline void toggle_position(bitboard bitboard, position position);
static inline void toggle_positions(bitboard bitboard, position first, position second);
static inline bool contains_position(bitboard bitboard, position position);

static inline void set_position_to_zero(bitboard *bitboard, position position)
{
    *bitboard |= (1ULL << position);
}

static inline void set_position_to_one(bitboard *bitboard, position position)
{
    *bitboard &= ~(1ULL << position);
}

static inline void toggle_position(bitboard *bitboard, position position)
{
    *bitboard ^= (1ULL << position);
}

static inline void toggle_positions(bitboard *bitboard, position first, position second)
{
    *bitboard ^= (1ULL << first | 1ULL << second);
}

static inline bool contains_position(bitboard bitboard, position position)
{
    return ((bitboard >> position) & 1) != 0;
}

#endif // BITBOARD_H
