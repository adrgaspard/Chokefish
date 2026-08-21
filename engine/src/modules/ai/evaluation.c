#include "evaluation.h"
#include "piece_values.h"

int32_t evaluation_v0(board *board)
{
    int32_t white_evaluation, black_evaluation, evaluation;
    white_evaluation = _count_material(board, COLOR_WHITE);
    black_evaluation = _count_material(board, COLOR_BLACK);
    evaluation = white_evaluation - black_evaluation;
    return board->color_to_move == COLOR_WHITE ? evaluation : -evaluation;
}

int32_t _count_material(board *board, color player)
{
    int32_t result = 0;
    result += board->piece_groups[player][PIECE_PAWN].count * PAWN_VALUE;
    result += board->piece_groups[player][PIECE_KNIGHT].count * KNIGHT_VALUE;
    result += board->piece_groups[player][PIECE_BISHOP].count * BISHOP_VALUE;
    result += board->piece_groups[player][PIECE_ROOK].count * ROOK_VALUE;
    result += board->piece_groups[player][PIECE_QUEEN].count * QUEEN_VALUE;
    return result;
}
