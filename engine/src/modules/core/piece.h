#ifndef PIECE_H
#define PIECE_H

#include "color.h"
#include "types.h"

#define _PIECE_MASK 0x0F
#define _PIECE_TYPE_MASK 0x07
#define _PIECE_COLOR_MASK 0x08
#define _PIECE_COLOR_BIT_INDEX 3
#define _GET_PIECE_COLOR(piece) ((piece & _PIECE_COLOR_MASK) >> _PIECE_COLOR_BIT_INDEX)
#define _GET_PIECE_TYPE(piece) ((piece_type)(piece & _PIECE_TYPE_MASK))

bool is_piece_type_valid(piece_type type);
bool is_piece_valid(piece piece);
piece create_piece(color color, piece_type type);
piece create_empty_piece();
color get_color(piece piece);
piece_type get_type(piece piece);
bool is_piece_empty(piece piece);

#endif // PIECE_H
