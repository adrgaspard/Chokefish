#ifndef PRECOMPUTED_BOARD_DATA_H
#define PRECOMPUTED_BOARD_DATA_H

#include "types.h"

extern bitboard g_position_mask[POSITIONS_COUNT];
extern bitboard g_rank_mask[RANKS_COUNT];
extern bitboard g_file_mask[FILES_COUNT];
extern bitboard g_not_file_mask[FILES_COUNT];
extern bitboard g_knight_attacks[POSITIONS_COUNT];
extern bitboard g_pawn_attacks[PLAYERS_COUNT][POSITIONS_COUNT];
extern bitboard g_king_moves[POSITIONS_COUNT];
extern bitboard g_orthogonal_moves_mask[POSITIONS_COUNT];
extern bitboard g_diagonal_moves_mask[POSITIONS_COUNT];
extern magic_data g_orthogonal_magic_data[POSITIONS_COUNT];
extern magic_data g_diagonal_magic_data[POSITIONS_COUNT];
extern int8_t g_directional_offset[DIRECTIONAL_OFFSETS_COUNT];
extern uint8_t g_distances_to_edge[POSITIONS_COUNT][DIRECTIONAL_OFFSETS_COUNT];
extern uint8_t g_orthogonal_distance[POSITIONS_COUNT][POSITIONS_COUNT];
extern uint8_t g_center_manhattan_distance[POSITIONS_COUNT];
extern bitboard g_align_mask[POSITIONS_COUNT][POSITIONS_COUNT];
extern bitboard g_directional_ray_mask[DIRECTIONAL_OFFSETS_COUNT][POSITIONS_COUNT];

void initialize_precomputed_board_data();
bitboard_dynamic_array compute_blockers_bitboards(bitboard moves_mask);
bitboard compute_moves_mask(position start_pos, bool ortho_instead_of_diag);
bitboard compute_legal_moves_mask(position start_pos, bitboard blockers_bitboard, bool ortho_instead_of_diag);
static inline bitboard get_slider_moves_bitboard(position position, bitboard blockers, bool ortho_instead_of_diag);
static inline bitboard get_orthogonal_moves_bitboard(position position, bitboard blockers);
static inline bitboard get_diagonal_moves_bitboard(position position, bitboard blockers);

static inline bitboard get_slider_moves_bitboard(position position, bitboard blockers, bool ortho_instead_of_diag)
{
    return ortho_instead_of_diag ? get_orthogonal_moves_bitboard(position, blockers) : get_diagonal_moves_bitboard(position, blockers);
}

static inline bitboard get_orthogonal_moves_bitboard(position position, bitboard blockers)
{
    bitboard key;
    magic_data *data;
    data = &(g_orthogonal_magic_data[position]);
    key = ((blockers & g_orthogonal_moves_mask[position]) * data->value) >> data->shift_quantity;
    return data->legal_moves[key];
}

static inline bitboard get_diagonal_moves_bitboard(position position, bitboard blockers)
{
    bitboard key;
    magic_data *data;
    data = &(g_diagonal_magic_data[position]);
    key = ((blockers & g_diagonal_moves_mask[position]) * data->value) >> data->shift_quantity;
    return data->legal_moves[key];
}

#endif // PRECOMPUTED_BOARD_DATA_H
