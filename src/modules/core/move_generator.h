#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "bitboard.h"
#include "color.h"
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

static inline void generate_moves(board *board, move_generation_result *result, move_generation_options options);
static inline void compute_attack_data(board *board, move_generation_data *data);
static inline void compute_sliding_attacks(board *board, move_generation_data *data, bitboard pieces_mask, bool ortho_instead_of_diag);
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
	data.opponent_sliding_attacks_mask = 0;
	compute_attack_data(board, &data);

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

static inline void compute_attack_data(board *board, move_generation_data *data)
{
	bool is_diagonal, is_friendly_piece_in_ray;
	bitboard slider_mask, ray_mask, knights_mask, knights_attacks_mask, current_king_mask, current_knight_attacks_mask, 
		pawns_mask, pawns_attacks_mask, pawn_possible_attack_origins_mask, pawn_check_mask;
	uint8_t start_direction_index, end_direction_index, direction_index, distance_to_edge, i;
	int8_t direction_offset;
	position pos, current_knight_pos, opponent_king_pos;
	piece piece;
	piece_type piece_type;
	// Compute orthogonal & diagonal attacks
	compute_sliding_attacks(board, data, board->orthogonal_sliders_mask[data->opponent_color], true);
	compute_sliding_attacks(board, data, board->diagonal_sliders_mask[data->opponent_color], false);

	// Search for pieces in any direction around the current king for eventual pins or checks
	start_direction_index = 0;
	end_direction_index = DIRECTIONAL_OFFSETS_COUNT;
	if (board->piece_groups[data->opponent_color][PIECE_QUEEN].count == 0)
	{
		start_direction_index = board->piece_groups[data->opponent_color][PIECE_ROOK].count > 0 ? 0 : ORTHO_OR_DIAG_OFFSETS_COUNT;
		end_direction_index = board->piece_groups[data->opponent_color][PIECE_BISHOP].count > 0 ? DIRECTIONAL_OFFSETS_COUNT : ORTHO_OR_DIAG_OFFSETS_COUNT;
	}
	for (direction_index = start_direction_index; direction_index < end_direction_index; direction_index++)
	{
		is_diagonal = direction_index >= ORTHO_OR_DIAG_OFFSETS_COUNT;
		slider_mask = is_diagonal ? board->diagonal_sliders_mask[data->opponent_color] : board->orthogonal_sliders_mask[data->opponent_color];
		if ((g_directional_ray_mask[direction_index][data->current_king_pos] & slider_mask) == 0)
		{
			continue;
		}
		distance_to_edge = g_distances_to_edge[data->current_king_pos][direction_index];
		direction_offset = g_directional_offset[direction_index];
		is_friendly_piece_in_ray = false;
		ray_mask = 0;
		for (i = 0; i < distance_to_edge; i++)
		{
			pos = (position)(data->current_king_pos + direction_offset * (i + 1));
			ray_mask |= 1ULL << pos;
			piece = board->position[pos];
			if (!is_piece_empty(piece))
			{
				// There is a piece on the ray
				if (get_color(piece) == data->current_color)
				{
					// There is a friendly piece on the ray
					if (is_friendly_piece_in_ray)
					{
						// Double blockers : pin is not possible
						break;
					}
					else
					{
						// Simple blockers : might be a pin
						is_friendly_piece_in_ray = true;
					}
				}
				else
				{
					// There is an enemy piece on the ray
					piece_type = get_type(piece);
					if (piece_type == PIECE_QUEEN || piece_type == (is_diagonal ? PIECE_BISHOP : PIECE_ROOK))
					{
						// The piece is in bitmask of pieces able to move in current direction
						if (is_friendly_piece_in_ray)
						{
							// There is a friendly piece blocking the attack : this is a pin
							data->pin_ray_mask |= ray_mask;
						}
						else
						{
							// There is no friendly piece blocking the attack : this is a check
							data->check_ray_mask |= ray_mask;
							data->is_currently_double_check = data->is_currently_check;
							data->is_currently_check = true;
						}
					}
					break;
				}
			}
		}
		if (data->is_currently_double_check)
		{
			// The king is the only piece that can move
			break;
		}
	}
	data->not_pin_ray_mask = ~data->pin_ray_mask;

	// Compute knight attacks
	knights_attacks_mask = 0;
	knights_mask = board->piece_masks[data->opponent_color][PIECE_KNIGHT];
	current_king_mask = 1ULL << data->current_king_pos;
	while (knights_mask != 0)
	{
		current_knight_pos = pop_least_significant_bit(&knights_mask);
		current_knight_attacks_mask = g_knight_attacks[current_knight_pos];
		knights_attacks_mask |= current_knight_attacks_mask;
		if ((current_knight_attacks_mask & current_king_mask) != 0)
		{
			data->is_currently_double_check = data->is_currently_check;
			data->is_currently_check = true;
			data->check_ray_mask |= 1ULL << current_knight_pos;
		}
	}

	// Compute pawn attacks
	pawns_mask = board->piece_masks[data->opponent_color][PIECE_PAWN];
	pawns_attacks_mask = get_pawn_bitboard_attacks(pawns_mask, data->opponent_color);
	if (contains_position(pawns_attacks_mask, data->current_king_pos))
	{
		data->is_currently_double_check = data->is_currently_check;
		data->is_currently_check = true;
		pawn_possible_attack_origins_mask = g_pawn_attacks[data->current_color][data->current_king_pos];
		pawn_check_mask = pawns_mask & pawn_possible_attack_origins_mask;
		data->check_ray_mask |= pawn_check_mask;
	}
	opponent_king_pos = board->king_position[data->opponent_color];
	data->opponent_all_except_pawns_attacks_mask = data->opponent_sliding_attacks_mask | knights_attacks_mask | g_king_moves[opponent_king_pos];
	data->opponent_all_attacks_mask = data->opponent_all_except_pawns_attacks_mask | pawns_attacks_mask;

	// Update check ray mask if not in check
	if (!data->is_currently_check)
	{
		data->check_ray_mask = UINT64_MAX;
	}
}

static inline void compute_sliding_attacks(board *board, move_generation_data *data, bitboard pieces_mask, bool ortho_instead_of_diag)
{
	bitboard blockers, moves;
	blockers = board->all_pieces_mask & ~(1ULL << data->current_king_pos);
	position position;
	while (pieces_mask != 0)
	{
		position = pop_least_significant_bit(&pieces_mask);
		moves = get_slider_moves_bitboard(position, blockers, ortho_instead_of_diag);
		data->opponent_sliding_attacks_mask |= moves;
	}
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
