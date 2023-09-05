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

static inline void generate_moves(board *board, move_generation_result *result, move_generation_options options);
static inline void _compute_attack_data(board *board, move_generation_data *data);
static inline void _compute_sliding_attacks(board *board, move_generation_data *data, bitboard pieces_mask, bool ortho_instead_of_diag);
static inline bool _is_position_pinned(bitboard pin_ray_mask, position position);
static inline void _generate_king_moves(board *board, move_generation_data *data, move_generation_result *result, move_generation_options options);
static inline void _generate_sliding_moves(board *board, move_generation_data *data, move_generation_result *result);
static inline void _generate_knight_moves(board *board, move_generation_data *data, move_generation_result *result);
static inline void _generate_pawn_moves(board *board, move_generation_data *data, move_generation_result *result, move_generation_options options);
static inline void _generate_pawn_promotions(position start_pos, position dest_pos, move_generation_result *result, move_generation_options options, bool is_capture);
static inline bool _is_check_after_en_passant(board *board, move_generation_data *data, position start_pos, position dest_pos, position capture_pos);

static inline void generate_moves(board *board, move_generation_result *result, move_generation_options options)
{
	move_generation_data data;
	assert(board != NULL);
	assert(result != NULL);

	// Initialization
	reset_move_generation_result(result);
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
	_compute_attack_data(board, &data);

	// Generate moves
	_generate_king_moves(board, &data, result, options);
	if (!data.is_currently_double_check)
	{
		_generate_sliding_moves(board, &data, result);
		_generate_knight_moves(board, &data, result);
		_generate_pawn_moves(board, &data, result, options);
	}

	// Update result secondary content
	result->is_currently_check = data.is_currently_check;
}

static inline void _compute_attack_data(board *board, move_generation_data *data)
{
	bool is_diagonal, is_friendly_piece_in_ray;
	bitboard slider_mask, ray_mask, knights_mask, knights_attacks_mask, current_king_mask, current_knight_attacks_mask, 
		pawns_mask, pawns_attacks_mask, pawn_possible_attack_origins_mask, pawn_check_mask;
	uint8_t start_direction_index, end_direction_index, direction_index, distance_to_edge, i;
	int8_t direction_offset;
	position pos, current_knight_pos, opponent_king_pos;
	piece piece;
	piece_type piece_type;
	assert(board != NULL);
	assert(data != NULL);

	// Compute orthogonal & diagonal attacks
	_compute_sliding_attacks(board, data, board->orthogonal_sliders_mask[data->opponent_color], true);
	_compute_sliding_attacks(board, data, board->diagonal_sliders_mask[data->opponent_color], false);

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
	pawns_attacks_mask = get_pawn_attacks_mask(pawns_mask, data->opponent_color);
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

static inline void _compute_sliding_attacks(board *board, move_generation_data *data, bitboard pieces_mask, bool ortho_instead_of_diag)
{
	bitboard blockers, moves;
	position position;
	assert(board != NULL);
	assert(data != NULL);
	blockers = board->all_pieces_mask & ~(1ULL << data->current_king_pos);
	while (pieces_mask != 0)
	{
		position = pop_least_significant_bit(&pieces_mask);
		moves = get_slider_moves_bitboard(position, blockers, ortho_instead_of_diag);
		data->opponent_sliding_attacks_mask |= moves;
	}
}

static inline bool _is_position_pinned(bitboard pin_ray_mask, position position)
{
	return ((pin_ray_mask >> position) & 1) != 0;
}

static inline void _generate_king_moves(board *board, move_generation_data *data, move_generation_result *result, move_generation_options options)
{
	bitboard legal_mask, moves_mask, castling_blockers_mask, castling_mask, castling_block_mask;
	position dest_pos;
	bool is_capture;
	castling castling_right;
	assert(board != NULL);
	assert(data != NULL);
	assert(result != NULL);
	legal_mask = ~(data->opponent_all_attacks_mask | data->friendly_pieces_mask);
	moves_mask = g_king_moves[data->current_king_pos] & legal_mask & data->allowed_destinations_mask;
	while (moves_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&moves_mask);
		is_capture = (data->enemy_pieces_mask & (1ULL << dest_pos)) != 0;
		result->moves[result->moves_count++] = create_movement(data->current_king_pos, dest_pos, is_capture ? MOVE_CAPTURE : MOVE_QUIET);
		assert(is_movement_valid(result->moves[result->moves_count - 1]));
		assert(!is_movement_empty(result->moves[result->moves_count - 1]));
	}
	
	// Castling
	if (!data->is_currently_check && options.include_quiet_moves)
	{
		castling_blockers_mask = data->opponent_all_attacks_mask | data->all_pieces_mask;
		castling_right = data->current_color == COLOR_WHITE
			? get_white_castling_right(board->current_game_state.castling_data)
			: get_black_castling_right(board->current_game_state.castling_data);
		assert(is_castling_valid(castling_right));
		if ((castling_right & CASTLING_KING) != 0)
		{
			castling_mask = data->current_color == COLOR_WHITE ? WHITE_KING_SIDE_CASTLE_MASK : BLACK_KING_SIDE_CASTLE_MASK;
			if ((castling_mask & castling_blockers_mask) == 0)
			{
				dest_pos = data->current_color == COLOR_WHITE ? POS_G1 : POS_G8;
				result->moves[result->moves_count++] = create_movement(data->current_king_pos, dest_pos, MOVE_KING_CASTLE);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			}
		}
		if ((castling_right & CASTLING_QUEEN) != 0)
		{
			castling_mask = data->current_color == COLOR_WHITE ? WHITE_QUEEN_SIDE_CASTLE_MASK : BLACK_QUEEN_SIDE_CASTLE_MASK;
			castling_block_mask = data->current_color == COLOR_WHITE ? WHITE_QUEEN_SIDE_CASTLE_BLOCK_MASK : BLACK_QUEEN_SIDE_CASTLE_BLOCK_MASK;
			if ((castling_mask & castling_blockers_mask) == 0 && (castling_block_mask & data->all_pieces_mask) == 0)
			{
				dest_pos = data->current_color == COLOR_WHITE ? POS_C1 : POS_C8;
				result->moves[result->moves_count++] = create_movement(data->current_king_pos, dest_pos, MOVE_QUEEN_CASTLE);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			}
		}
	}
}

static inline void _generate_sliding_moves(board *board, move_generation_data *data, move_generation_result *result)
{
	bitboard moves_mask, orthogonal_sliders_mask, diagonal_sliders_mask, orthogonal_moves_mask, diagonal_moves_mask;
	position start_pos, dest_pos;
	bool is_capture;
	assert(board != NULL);
	assert(data != NULL);
	assert(result != NULL);

	// Initialization
	moves_mask = data->empty_pos_or_enemy_pieces_mask & data->check_ray_mask & data->allowed_destinations_mask;
	orthogonal_sliders_mask = board->orthogonal_sliders_mask[data->current_color];
	diagonal_sliders_mask = board->diagonal_sliders_mask[data->current_color];
	if (data->is_currently_check)
	{
		// Remove pinned pieces from sliders
		orthogonal_sliders_mask &= ~data->pin_ray_mask;
		diagonal_sliders_mask &= ~data->pin_ray_mask;
	}

	// Orthogonal moves
	while (orthogonal_sliders_mask != 0)
	{
		start_pos = pop_least_significant_bit(&orthogonal_sliders_mask);
		orthogonal_moves_mask = get_orthogonal_moves_bitboard(start_pos, data->all_pieces_mask) & moves_mask;
		if (_is_position_pinned(data->pin_ray_mask, start_pos))
		{
			orthogonal_moves_mask &= g_align_mask[start_pos][data->current_king_pos];
		}
		while (orthogonal_moves_mask != 0)
		{
			dest_pos = pop_least_significant_bit(&orthogonal_moves_mask);
			is_capture = (data->enemy_pieces_mask & (1ULL << dest_pos)) != 0;
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_CAPTURE : MOVE_QUIET);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
		}
	}

	// Diagonal moves
	while (diagonal_sliders_mask != 0)
	{
		start_pos = pop_least_significant_bit(&diagonal_sliders_mask);
		diagonal_moves_mask = get_diagonal_moves_bitboard(start_pos, data->all_pieces_mask) & moves_mask;
		if (_is_position_pinned(data->pin_ray_mask, start_pos))
		{
			diagonal_moves_mask &= g_align_mask[start_pos][data->current_king_pos];
		}
		while (diagonal_moves_mask != 0)
		{
			dest_pos = pop_least_significant_bit(&diagonal_moves_mask);
			is_capture = (data->enemy_pieces_mask & (1ULL << dest_pos)) != 0;
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_CAPTURE : MOVE_QUIET);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
		}
	}
}

static inline void _generate_knight_moves(board *board, move_generation_data *data, move_generation_result *result)
{
	bitboard knights_mask, moves_mask, current_knight_moves_mask;
	position start_pos, dest_pos;
	bool is_capture;
	assert(board != NULL);
	assert(data != NULL);
	assert(result != NULL);
	knights_mask = board->piece_masks[data->current_color][PIECE_KNIGHT] & data->not_pin_ray_mask;
	moves_mask = data->empty_pos_or_enemy_pieces_mask & data->check_ray_mask & data->allowed_destinations_mask;
	while (knights_mask != 0)
	{
		start_pos = pop_least_significant_bit(&knights_mask);
		current_knight_moves_mask = g_knight_attacks[start_pos] & moves_mask;
		while (current_knight_moves_mask != 0)
		{
			dest_pos = pop_least_significant_bit(&current_knight_moves_mask);
			is_capture = (data->enemy_pieces_mask & (1ULL << dest_pos)) != 0;
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_CAPTURE : MOVE_QUIET);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
		}
	}
}

static inline void _generate_pawn_moves(board *board, move_generation_data *data, move_generation_result *result, move_generation_options options)
{
	int8_t push_direction, push_offset, en_passant_rank;
	bitboard pawns_mask, promotion_rank_mask, single_pushes_mask, promotion_pushes_mask, first_capture_edge_file_mask, second_capture_edge_file_mask,
		first_capture_mask, second_capture_mask, no_promotion_single_pushes_mask, first_promotion_capture_mask, second_promotion_capture_mask,
		double_push_dest_rank_mask, double_pushes_mask, en_passant_potential_attackers_mask;
	position start_pos, dest_pos, en_passant_capture_pos;
	assert(board != NULL);
	assert(data != NULL);
	assert(result != NULL);

	// Initialization
	push_direction = data->current_color == COLOR_WHITE ? 1 : -1;
	push_offset = (int8_t)(push_direction * FILES_COUNT);
	pawns_mask = board->piece_masks[data->current_color][PIECE_PAWN];
	promotion_rank_mask = data->current_color == COLOR_WHITE ? g_rank_mask[RANKS_COUNT - 1] : g_rank_mask[0];
	single_pushes_mask = shift(pawns_mask, push_offset) & data->empty_pos_mask;
	promotion_pushes_mask = single_pushes_mask & promotion_rank_mask & data->check_ray_mask;
	first_capture_edge_file_mask = data->current_color == COLOR_WHITE ? g_not_file_mask[0] : g_not_file_mask[FILES_COUNT - 1];
	second_capture_edge_file_mask = data->current_color == COLOR_WHITE ? g_not_file_mask[FILES_COUNT - 1] : g_not_file_mask[0];
	first_capture_mask = shift(pawns_mask & first_capture_edge_file_mask, (int8_t)(push_direction * (FILES_COUNT - 1))) & data->enemy_pieces_mask;
	second_capture_mask = shift(pawns_mask & second_capture_edge_file_mask, (int8_t)(push_direction * (FILES_COUNT + 1))) & data->enemy_pieces_mask;
	no_promotion_single_pushes_mask = single_pushes_mask & ~promotion_rank_mask & data->check_ray_mask;
	first_promotion_capture_mask = first_capture_mask & promotion_rank_mask & data->check_ray_mask;
	second_promotion_capture_mask = second_capture_mask & promotion_rank_mask & data->check_ray_mask;
	first_capture_mask &= ~promotion_rank_mask & data->check_ray_mask;
	second_capture_mask &= ~promotion_rank_mask & data->check_ray_mask;

	// Single and double pushes (quiet moves, no promotion)
	if (options.include_quiet_moves)
	{
		// Single pushes
		while (no_promotion_single_pushes_mask != 0)
		{
			dest_pos = pop_least_significant_bit(&no_promotion_single_pushes_mask);
			start_pos = (int8_t)(dest_pos - push_offset);
			if (!_is_position_pinned(data->pin_ray_mask, start_pos)
				|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
			{
				result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, MOVE_QUIET);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			}
		}
		// Double pushes
		double_push_dest_rank_mask = data->current_color == COLOR_WHITE ? g_rank_mask[3] : g_rank_mask[RANKS_COUNT - 4];
		double_pushes_mask = shift(single_pushes_mask, push_offset) & data->empty_pos_mask & double_push_dest_rank_mask & data->check_ray_mask;
		while (double_pushes_mask != 0)
		{
			dest_pos = pop_least_significant_bit(&double_pushes_mask);
			start_pos = (int8_t)(dest_pos - push_offset * 2);
			if (!_is_position_pinned(data->pin_ray_mask, start_pos)
				|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
			{
				result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, MOVE_DOUBLE_PAWN_PUSH);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			}
		}
	}

	// Captures (no promotion, no en passant)
	while (first_capture_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&first_capture_mask);
		start_pos = (int8_t)(dest_pos - push_direction * (FILES_COUNT - 1));
		if (!_is_position_pinned(data->pin_ray_mask, start_pos)
			|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
		{
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, MOVE_CAPTURE);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
		}
	}
	while (second_capture_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&second_capture_mask);
		start_pos = (int8_t)(dest_pos - push_direction * (FILES_COUNT + 1));
		if (!_is_position_pinned(data->pin_ray_mask, start_pos)
			|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
		{
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, MOVE_CAPTURE);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
		}
	}

	// All promotions (with capture or not)
	while (promotion_pushes_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&promotion_pushes_mask);
		start_pos = (int8_t)(dest_pos - push_offset);
		if (!_is_position_pinned(data->pin_ray_mask, start_pos))
		{
			_generate_pawn_promotions(start_pos, dest_pos, result, options, false);
		}
	}
	while (first_promotion_capture_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&first_promotion_capture_mask);
		start_pos = (int8_t)(dest_pos - push_direction * (FILES_COUNT - 1));
		if (!_is_position_pinned(data->pin_ray_mask, start_pos)
			|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
		{
			_generate_pawn_promotions(start_pos, dest_pos, result, options, true);
		}
	}
	while (second_promotion_capture_mask != 0)
	{
		dest_pos = pop_least_significant_bit(&second_promotion_capture_mask);
		start_pos = (int8_t)(dest_pos - push_direction * (FILES_COUNT + 1));
		if (!_is_position_pinned(data->pin_ray_mask, start_pos)
			|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
		{
			_generate_pawn_promotions(start_pos, dest_pos, result, options, true);
		}
	}

	// En passant capture
	if (board->current_game_state.last_en_passant_file != NO_FILE)
	{
		en_passant_rank = data->current_color == COLOR_WHITE ? RANKS_COUNT - 3 : 2;
		dest_pos = compute_index_if_valid(board->current_game_state.last_en_passant_file, en_passant_rank).index;
		en_passant_capture_pos = (int8_t)(dest_pos - push_offset);
		if (contains_position(data->check_ray_mask, en_passant_capture_pos))
		{
			en_passant_potential_attackers_mask = pawns_mask & get_pawn_attacks_mask(1ULL << dest_pos, data->opponent_color);
			while (en_passant_potential_attackers_mask != 0)
			{
				start_pos = pop_least_significant_bit(&en_passant_potential_attackers_mask);
				if (!_is_position_pinned(data->pin_ray_mask, start_pos)
					|| g_align_mask[start_pos][data->current_king_pos] == g_align_mask[dest_pos][data->current_king_pos])
				{
					if (!_is_check_after_en_passant(board, data, start_pos, dest_pos, en_passant_capture_pos))
					{
						result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, MOVE_EN_PASSANT_CAPTURE);
						assert(is_movement_valid(result->moves[result->moves_count - 1]));
						assert(!is_movement_empty(result->moves[result->moves_count - 1]));
					}
				}
			}
		}
	}
}

static inline void _generate_pawn_promotions(position start_pos, position dest_pos, move_generation_result *result, move_generation_options options, bool is_capture)
{
	assert(is_position_valid(start_pos) && start_pos != NO_POSITION);
	assert(is_position_valid(dest_pos) && dest_pos != NO_POSITION);
	assert(result != NULL);
	result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_QUEEN_PROMOTION_CAPTURE : MOVE_QUEEN_PROMOTION);
	assert(is_movement_valid(result->moves[result->moves_count - 1]));
	assert(!is_movement_empty(result->moves[result->moves_count - 1]));
	// Only include one variation (queen) when quiet moves are disabled
	if (options.include_quiet_moves)
	{
		if ((options.promotion_types_to_include & PROMOTION_KIGHT) != 0)
		{
			result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_KNIGHT_PROMOTION_CAPTURE : MOVE_KNIGHT_PROMOTION);
			assert(is_movement_valid(result->moves[result->moves_count - 1]));
			assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			if (options.promotion_types_to_include == PROMOTION_ALL)
			{
				result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_ROOK_PROMOTION_CAPTURE : MOVE_ROOK_PROMOTION);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
				result->moves[result->moves_count++] = create_movement(start_pos, dest_pos, is_capture ? MOVE_BISHOP_PROMOTION_CAPTURE : MOVE_BISHOP_PROMOTION);
				assert(is_movement_valid(result->moves[result->moves_count - 1]));
				assert(!is_movement_empty(result->moves[result->moves_count - 1]));
			}
		}
	}
}

static inline bool _is_check_after_en_passant(board *board, move_generation_data *data, position start_pos, position dest_pos, position capture_pos)
{
	bitboard opponent_ortho_sliders_mask, previous_blockers_mask, orthogonal_attacks_mask;
	assert(board != NULL);
	assert(data != NULL);
	assert(is_position_valid(start_pos) && start_pos != NO_POSITION);
	assert(is_position_valid(dest_pos) && dest_pos != NO_POSITION);
	assert(is_position_valid(capture_pos) && capture_pos != NO_POSITION);
	opponent_ortho_sliders_mask = board->orthogonal_sliders_mask[data->opponent_color];
	if (opponent_ortho_sliders_mask != 0)
	{
		previous_blockers_mask = (data->all_pieces_mask ^ (1ULL << capture_pos | 1ULL << start_pos | 1ULL << dest_pos));
		orthogonal_attacks_mask = get_orthogonal_moves_bitboard(data->current_king_pos, previous_blockers_mask);
		return (orthogonal_attacks_mask & opponent_ortho_sliders_mask) != 0;
	}
	return false;
}

#endif // MOVE_GENERATOR_H
