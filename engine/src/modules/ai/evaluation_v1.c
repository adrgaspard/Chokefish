#include "../core/color.h"
#include "../core/precomputed_board_data.h"
#include "evaluation.h"
#include "evaluation_v1.h"

#define KING_PROXIMITY_MULTIPLIER 10
#define MAX_KING_DISTANCE 14

int32_t evaluation_v1(board *board)
{
    int32_t white_evaluation, black_evaluation, evaluation, proximity_bonus;
    color winning_color, losing_color;
    white_evaluation = _count_material(board, COLOR_WHITE);
    black_evaluation = _count_material(board, COLOR_BLACK);
    evaluation = white_evaluation - black_evaluation;
    if (white_evaluation != black_evaluation)
    {
        // The proximity term only applies in true endgames: the losing side has no
        // non-pawn material left, so the winning king must approach to force promotion or mate
        losing_color = white_evaluation > black_evaluation ? COLOR_BLACK : COLOR_WHITE;
        if (board->piece_groups[losing_color][PIECE_KNIGHT].count == 0
            && board->piece_groups[losing_color][PIECE_BISHOP].count == 0
            && board->piece_groups[losing_color][PIECE_ROOK].count == 0
            && board->piece_groups[losing_color][PIECE_QUEEN].count == 0)
        {
            winning_color = get_opponent(losing_color);
            proximity_bonus = (MAX_KING_DISTANCE - g_orthogonal_distance[board->king_position[winning_color]][board->king_position[losing_color]]) * KING_PROXIMITY_MULTIPLIER;
            evaluation += winning_color == COLOR_WHITE ? proximity_bonus : -proximity_bonus;
        }
    }
    return board->color_to_move == COLOR_WHITE ? evaluation : -evaluation;
}
