#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include "game_state.h"
#include "move.h"
#include "piece_group.h"
#include "position.h"
#include "precomputed_board_data.h"
#include "zobrist.h"

#define _WHITE_EN_PASSANT_CAPTURE_GAP -RANKS_COUNT
#define _BLACK_EN_PASSANT_CAPTURE_GAP RANKS_COUNT
#define _CASTLING_KING_SIDE_ROOK_START(dest_pos) dest_pos + 1
#define _CASTLING_QUEEN_SIDE_ROOK_START(dest_pos) dest_pos - 2
#define _CASTLING_KING_SIDE_ROOK_DEST(dest_pos) dest_pos - 1
#define _CASTLING_QUEEN_SIDE_ROOK_DEST(dest_pos) dest_pos + 1

void create_board(board *board);
bool is_check(board *board);
bool compute_check_state(board *board);
void update_sliders(board *board);
void move_piece(board *board, color color, piece piece, piece_type piece_type, position start_pos, position dest_pos);
void do_move(board *board, move move, bool is_search);
void undo_move(board *board, move move, bool is_search);
void do_null_move(board *board);
void undo_null_move(board *board);

#endif // BOARD_H
