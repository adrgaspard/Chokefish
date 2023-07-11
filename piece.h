#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>
#include "color.h"

#define PIECE_TYPES_COUNT 6
#define _PIECE_MASK 0b00001111
#define _PIECE_TYPE_MASK 0b00000111
#define _PIECE_COLOR_MASK 0b00001000
#define _PIECE_COLOR_BIT_INDEX 3

typedef enum piece_type
{
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5,
    PIECE_NONE = 6
} piece_type;

typedef uint8_t piece;

static inline bool is_piece_type_valid(piece_type type)
{
    return type >= PIECE_NONE && type <= PIECE_QUEEN;
}

static inline bool is_piece_valid(piece piece)
{
    return is_piece_type_valid(get_type(piece)) && is_color_valid(get_color(piece));
}

static inline piece create_piece(color color, piece_type type)
{
    return (color << _PIECE_COLOR_BIT_INDEX) | type;
}

static inline piece create_empty_piece()
{
    return create_piece(COLOR_WHITE, PIECE_NONE);
}

static inline color get_color(piece piece)
{
    return piece & _PIECE_COLOR_MASK;
}

static inline piece_type get_type(piece piece)
{
    return piece & _PIECE_TYPE_MASK;
}

static inline bool is_piece_empty(piece piece)
{
    return get_type(piece) == PIECE_NONE;
}

#endif // PIECE_H
