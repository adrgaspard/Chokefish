#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

void set_position_to_zero(bitboard *bitboard, position position);
void set_position_to_one(bitboard *bitboard, position position);
void toggle_position(bitboard *bitboard, position position);
void toggle_positions(bitboard *bitboard, position first, position second);
bool contains_position(bitboard bitboard, position position);
int8_t pop_least_significant_bit(bitboard *bitboard);
bitboard shift(bitboard bitboard, int8_t shift_quantity);
bitboard get_pawn_attacks_mask(bitboard pawn_bitboard, color color);

#endif // BITBOARD_H
