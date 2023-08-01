#ifndef PRECOMPUTED_BOARD_DATA_H
#define PRECOMPUTED_BOARD_DATA_H

#include "dynamic_collections.h"
#include "types.h"

DECLARE_DYNAMIC_ARRAY_TYPE(bitboard, bitboard_dynamic_array)

bitboard g_position_mask[POSITIONS_COUNT];
bitboard g_rank_mask[RANKS_COUNT];
bitboard g_file_mask[FILES_COUNT];
bitboard g_not_file_mask[FILES_COUNT];
bitboard g_knight_attacks[POSITIONS_COUNT];
bitboard g_pawn_attacks[PLAYERS_COUNT][POSITIONS_COUNT];
bitboard g_king_moves[POSITIONS_COUNT];

void initialize_precomputed_board_data();
bitboard_dynamic_array compute_blockers_bitboards(bitboard move_mask);
bitboard compute_moves_mask(position start_pos, bool ortho_instead_of_diag);
bitboard compute_legal_moves_mask(position start_pos, bitboard blockers_bitboard, bool ortho_instead_of_diag);

#endif // PRECOMPUTED_BOARD_DATA_H
