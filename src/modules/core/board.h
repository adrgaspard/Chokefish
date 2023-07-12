#ifndef BOARD_H
#define BOARD_H

#include "types.h"

bitboard g_position_mask[POSITIONS_COUNT];
bitboard g_rank_mask[RANKS_COUNT];
bitboard g_file_mask[FILES_COUNT];

void initialize_board_component();
static inline bool is_valid_board(board *board);
static inline void do_move(board *board, move move);

static inline bool is_valid_board(board *board)
{
    (void)board;
    // TODO
    return true;
}

static inline void do_move(board *board, move move)
{
    (void)board;
    (void)move;
    // TODO
}

#endif // BOARD_H
