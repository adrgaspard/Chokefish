#ifndef BOARD_H
#define BOARD_H

#include "types.h"

board create_board();
bool is_check(board *board);
bool compute_check_state(board *board);
void update_sliders(board *board);
void move_piece(board *board, color color, piece piece, piece_type piece_type, position start_pos, position dest_pos);
void do_move(board *board, move move, bool is_search);
void undo_move(board *board, move move, bool is_search);
void do_null_move(board *board);
void undo_null_move(board *board);

#endif // BOARD_H
