#ifndef PIECE_H
#define PIECE_H

#include "color.h"
#include "types.h"

#define _PIECE_MASK 0x0F
#define _PIECE_TYPE_MASK 0x07
#define _PIECE_COLOR_MASK 0x08
#define _PIECE_COLOR_BIT_INDEX 3

static inline bool is_piece_type_valid(piece_type type);
static inline bool is_piece_valid(piece piece);
static inline piece create_piece(color color, piece_type type);
static inline piece create_empty_piece();
static inline color get_color(piece piece);
static inline piece_type get_type(piece piece);
static inline bool is_piece_empty(piece piece);

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
    assert(is_color_valid(color) && color != COLOR_NONE);
    assert(is_piece_type_valid(type) && type != PIECE_NONE);
    return (piece)((color << _PIECE_COLOR_BIT_INDEX) | type);
}

static inline piece create_empty_piece()
{
    return (piece)((COLOR_WHITE << _PIECE_COLOR_BIT_INDEX) | PIECE_NONE);
}

static inline color get_color(piece piece)
{
    assert(is_piece_valid(piece));
    return (color)((piece & _PIECE_COLOR_MASK) >> _PIECE_COLOR_BIT_INDEX);
}

static inline piece_type get_type(piece piece)
{
    assert(is_piece_valid(piece));
    return (piece_type)(piece & _PIECE_TYPE_MASK);
}

static inline bool is_piece_empty(piece piece)
{
    assert(is_piece_valid(piece));
    return get_type(piece) == PIECE_NONE;
}

#endif // PIECE_H
