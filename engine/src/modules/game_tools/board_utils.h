#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

#include "../core/board.h"

void reset_board(board *board, char *fen);
void copy_board(board *source, board *destination);

#endif // BOARD_UTILS_H
