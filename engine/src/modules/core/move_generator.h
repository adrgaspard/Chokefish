#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "castling.h"
#include "color.h"
#include "move.h"
#include "move_generation_result.h"
#include "piece.h"
#include "precomputed_board_data.h"
#include "types.h"

typedef struct move_generation_data
{
	bool is_currently_check;
	bool is_currently_double_check;
	bitboard check_ray_mask;
	bitboard pin_ray_mask;
	bitboard not_pin_ray_mask;
	bitboard friendly_pieces_mask;
	bitboard enemy_pieces_mask;
	bitboard all_pieces_mask;
	bitboard empty_pos_mask;
	bitboard empty_pos_or_enemy_pieces_mask;
	bitboard allowed_destinations_mask;
	bitboard opponent_sliding_attacks_mask;
	bitboard opponent_all_attacks_mask;
	bitboard opponent_all_except_pawns_attacks_mask;
	color current_color;
	color opponent_color;
	position current_king_pos;
} move_generation_data;

void generate_moves(board *board, move_generation_result *result, move_generation_options options);
void generate_moves_with_data(board *board, move_generation_result *result, move_generation_options options, move_generation_data *data);

#endif // MOVE_GENERATOR_H
