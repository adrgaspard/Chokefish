#ifndef PRECOMPUTED_BOARD_DATA_H
#define PRECOMPUTED_BOARD_DATA_H

#include "types.h"

bitboard g_position_mask[POSITIONS_COUNT];
bitboard g_rank_mask[RANKS_COUNT];
bitboard g_file_mask[FILES_COUNT];
bitboard g_not_file_mask[FILES_COUNT];
bitboard g_knight_attacks[POSITIONS_COUNT];
bitboard g_pawn_attacks[PLAYERS_COUNT][POSITIONS_COUNT];
bitboard g_king_moves[POSITIONS_COUNT];

void initialize_precomputed_board_data();

#endif // PRECOMPUTED_BOARD_DATA_H
