#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "color.h"
#include "types.h"

typedef struct move_generation_data
{
	bool is_currently_check;
	bool is_currently_double_check;
	bitboard check_ray_mask;
	bitboard pin_ray_mask;
	bitboard friendly_pieces_mask;
	bitboard enemy_pieces_mask;
	bitboard all_pieces_mask;
	bitboard empty_pos_mask;
	bitboard empty_pos_or_enemy_pieces_mask;
	bitboard allowed_destinations_mask;
	color current_color;
	color opponent_color;
	position current_king_pos;
} move_generation_data;

static inline void generate_moves(board *board, move_generation_result *result, move_generation_options options);
static inline void compute_attack_data();
static inline void generate_king_moves();
static inline void generate_sliding_moves();
static inline void generate_knight_moves();
static inline void generate_pawn_moves();

static inline void generate_moves(board *board, move_generation_result *result, move_generation_options options)
{
	move_generation_data data;

	// Initialization
	result->moves_count = 0;
	data.is_currently_check = false;
	data.is_currently_double_check = false;
	data.check_ray_mask = 0;
	data.pin_ray_mask = 0;
	data.current_color = board->color_to_move;
	data.opponent_color = get_opponent(data.current_color);
	data.current_king_pos = board->king_position[data.current_color];
	data.friendly_pieces_mask = board->color_mask[data.current_color];
	data.enemy_pieces_mask = board->color_mask[data.opponent_color];
	data.all_pieces_mask = board->all_pieces_mask;
	data.empty_pos_mask = ~data.all_pieces_mask;
	data.empty_pos_or_enemy_pieces_mask = data.empty_pos_mask | data.enemy_pieces_mask;
	data.allowed_destinations_mask = options.include_quiet_moves ? UINT64_MAX : data.enemy_pieces_mask;
	compute_attack_data();

	// Generate moves
	generate_king_moves();
	if (!data.is_currently_double_check)
	{
		generate_sliding_moves();
		generate_knight_moves();
		generate_pawn_moves();
	}

	// Update result secondary content
	result->is_currently_check = data.is_currently_check;
}

static inline void compute_attack_data()
{
	// TODO
}

static inline void generate_king_moves()
{
	// TODO
}

static inline void generate_sliding_moves()
{
	// TODO
}

static inline void generate_knight_moves()
{
	// TODO
}

static inline void generate_pawn_moves()
{
	// TODO
}

#endif // MOVE_GENERATOR_H
