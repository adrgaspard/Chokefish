#ifndef BOARD_H
#define BOARD_H

#include "precomputed_board_data.h"
#include "types.h"

static inline bool is_valid_board(board *board);
static inline board create_board();
static inline void do_move(board *board, move move);

static inline bool is_valid_board(board *board)
{
    (void)board;
    // TODO
    return true;
}

static inline board create_board()
{
    board board;
    // TODO
    return board;
}

static inline void do_move(board *board, move move)
{
    (void)board;
    (void)move;
    // TODO
}

#endif // BOARD_H
