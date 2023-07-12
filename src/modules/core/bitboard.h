#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

static inline bitboard set_position_to_zero(bitboard bitboard, position position);
static inline bitboard set_position_to_one(bitboard bitboard, position position);
static inline bitboard toggle_position(bitboard bitboard, position position);
static inline bitboard toggle_positions(bitboard bitboard, position first, position second);
static inline bitboard contains_position(bitboard bitboard, position position);

static inline bitboard set_position_to_zero(bitboard bitboard, position position)
{
    return bitboard | (1ULL << position);
}

static inline bitboard set_position_to_one(bitboard bitboard, position position)
{
    return bitboard & ~(1ULL << position);
}

static inline bitboard toggle_position(bitboard bitboard, position position)
{
    return bitboard ^ (1ULL << position);
}

static inline bitboard toggle_positions(bitboard bitboard, position first, position second)
{
    return bitboard ^ (1ULL << first | 1ULL << second);
}

static inline bitboard contains_position(bitboard bitboard, position position)
{
    return ((bitboard >> position) & 1) != 0;
}

#endif // BITBOARD_H
