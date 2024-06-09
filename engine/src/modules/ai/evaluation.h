#ifndef EVALUATION_H
#define EVALUATION_H

#include "../core/types.h"

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 320
#define ROOK_VALUE 525
#define QUEEN_VALUE 950

static inline int32_t evaluation_v0(board *board);
static inline int32_t _count_material(board *board, color player);

static inline int32_t evaluation_v0(board *board) {
    int32_t white_evaluation, black_evaluation, evaluation;
    white_evaluation = _count_material(board, COLOR_WHITE);
    black_evaluation = _count_material(board, COLOR_BLACK);
    evaluation = white_evaluation - black_evaluation;
    return board->color_to_move == COLOR_WHITE ? evaluation : -evaluation;
}

static inline int32_t _count_material(board *board, color player) {
    int32_t result = 0;
    result += board->piece_groups[player][PIECE_PAWN].count * PAWN_VALUE;
    result += board->piece_groups[player][PIECE_KNIGHT].count * KNIGHT_VALUE;
    result += board->piece_groups[player][PIECE_BISHOP].count * BISHOP_VALUE;
    result += board->piece_groups[player][PIECE_ROOK].count * ROOK_VALUE;
    result += board->piece_groups[player][PIECE_QUEEN].count * QUEEN_VALUE;
    return result;
}

#endif // EVALUATION_H
