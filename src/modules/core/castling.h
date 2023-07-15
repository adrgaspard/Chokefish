#ifndef CASTLING_H
#define CASTLING_H

#include "types.h"

#define _CASTLING_DATA_MASK 0xF
#define _CASTLING_MASK 0x3
#define _CASTLING_BIT_SIZE 2
#define _BLACK_CASTLING_MASK 0x3
#define _WHITE_CASTLING_MASK 0xC
#define _BLACK_AND_KING_WHITE_CASTLING_MASK 0x7
#define _BLACK_AND_QUEEN_WHITE_CASTLING_MASK 0xB
#define _WHITE_AND_KING_WHITE_CASTLING_MASK 0xD
#define _WHITE_AND_QUEEN_WHITE_CASTLING_MASK 0xE

static inline bool is_castling_valid(castling castling);
static inline bool is_castling_data_valid(castling_data data);
static inline castling_data create_castling_data(castling white_castling_right, castling black_castling_right);
static inline castling get_white_castling_right(castling_data data);
static inline castling get_black_castling_right(castling_data data);
static inline castling get_castling_data_without_both_sides(castling_data data, color color);
static inline castling get_castling_data_without_king_side(castling_data data, color color);
static inline castling get_castling_data_without_queen_side(castling_data data, color color);

static inline bool is_castling_valid(castling castling)
{
    return castling >= CASTLING_NONE && castling <= CASTLING_BOTH;
}

static inline bool is_castling_data_valid(castling_data data)
{
    return data <= _CASTLING_DATA_MASK;
}

static inline castling_data create_castling_data(castling white_castling_right, castling black_castling_right)
{
    return (castling_data)((white_castling_right << _CASTLING_BIT_SIZE) | black_castling_right);
}

static inline castling get_white_castling_right(castling_data data)
{
    return (castling)((data >> _CASTLING_BIT_SIZE) & _CASTLING_MASK);
}

static inline castling get_black_castling_right(castling_data data)
{
    return (castling)(data & _CASTLING_MASK);
}

static inline castling get_castling_data_without_both_sides(castling_data data, color color)
{
    return (castling)(data & (color == COLOR_WHITE ? _BLACK_CASTLING_MASK : _WHITE_CASTLING_MASK));
}

static inline castling get_castling_data_without_king_side(castling_data data, color color)
{
    return (castling)(data & (color == COLOR_WHITE ? _BLACK_AND_QUEEN_WHITE_CASTLING_MASK : _WHITE_AND_QUEEN_WHITE_CASTLING_MASK));
}

static inline castling get_castling_data_without_queen_side(castling_data data, color color)
{
    return (castling)(data & (color == COLOR_WHITE ? _BLACK_AND_KING_WHITE_CASTLING_MASK : _WHITE_AND_KING_WHITE_CASTLING_MASK));
}

#endif // CASTLING_H
