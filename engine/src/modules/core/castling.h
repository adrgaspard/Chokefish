#ifndef CASTLING_H
#define CASTLING_H

#include "types.h"

bool is_castling_valid(castling castling);
bool is_castling_data_valid(castling_data data);
castling_data create_castling_data(castling white_castling_right, castling black_castling_right);
castling get_white_castling_right(castling_data data);
castling get_black_castling_right(castling_data data);
castling_data get_castling_data_without_both_sides(castling_data data, color color);
castling_data get_castling_data_without_king_side(castling_data data, color color);
castling_data get_castling_data_without_queen_side(castling_data data, color color);

#endif // CASTLING_H
