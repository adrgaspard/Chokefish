#ifndef PIECE_H
#define PIECE_H

#include "types.h"

bool is_piece_type_valid(piece_type type);
bool is_piece_valid(piece piece);
piece create_piece(color color, piece_type type);
piece create_empty_piece();
color get_color(piece piece);
piece_type get_type(piece piece);
bool is_piece_empty(piece piece);

#endif // PIECE_H
