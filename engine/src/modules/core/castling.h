#ifndef CASTLING_H
#define CASTLING_H

#include "color.h"
#include "types.h"

#define _CASTLING_DATA_MASK 0xF
#define _CASTLING_MASK 0x3
#define _CASTLING_BIT_SIZE 2
#define _BLACK_CASTLING_MASK 0x3
#define _WHITE_CASTLING_MASK 0xC
#define _BLACK_AND_KING_WHITE_CASTLING_MASK 0x7
#define _BLACK_AND_QUEEN_WHITE_CASTLING_MASK 0xB
#define _WHITE_AND_KING_BLACK_CASTLING_MASK 0xD
#define _WHITE_AND_QUEEN_BLACK_CASTLING_MASK 0xE

bool is_castling_valid(castling castling);
bool is_castling_data_valid(castling_data data);
castling_data create_castling_data(castling white_castling_right, castling black_castling_right);
castling get_white_castling_right(castling_data data);
castling get_black_castling_right(castling_data data);
castling_data get_castling_data_without_both_sides(castling_data data, color color);
castling_data get_castling_data_without_king_side(castling_data data, color color);
castling_data get_castling_data_without_queen_side(castling_data data, color color);

#endif // CASTLING_H
