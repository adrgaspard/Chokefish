#include "../core/color.h"
#include "../core/precomputed_board_data.h"
#include "evaluation.h"
#include "evaluation_v2.h"

#define KING_PROXIMITY_MULTIPLIER 10
#define MAX_KING_DISTANCE 14

// Piece-square tables, from the owning side's perspective: row 0 is the side's first rank.
// The black side mirrors the table by flipping the rank (index = position ^ 56).
static const int16_t s_pawn_table[POSITIONS_COUNT] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

static const int16_t s_knight_table[POSITIONS_COUNT] =
{
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

static const int16_t s_bishop_table[POSITIONS_COUNT] =
{
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

static const int16_t s_rook_table[POSITIONS_COUNT] =
{
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

static const int16_t s_queen_table[POSITIONS_COUNT] =
{
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

static int32_t count_piece_square_value(board *board, color player)
{
    int32_t result = 0;
    uint8_t i;
    piece_type type;
    piece_group *group;
    const int16_t *table;
    for (type = PIECE_PAWN; type < PIECE_TYPES_COUNT - 1; type++)
    {
        switch (type)
        {
            case PIECE_PAWN: table = s_pawn_table; break;
            case PIECE_KNIGHT: table = s_knight_table; break;
            case PIECE_BISHOP: table = s_bishop_table; break;
            case PIECE_ROOK: table = s_rook_table; break;
            case PIECE_QUEEN: table = s_queen_table; break;
            default: continue;
        }
        group = &(board->piece_groups[player][type]);
        for (i = 0; i < group->count; i++)
        {
            result += table[player == COLOR_WHITE ? group->positions[i] : (group->positions[i] ^ 56)];
        }
    }
    return result;
}

int32_t evaluation_v2(board *board)
{
    int32_t white_evaluation, black_evaluation, evaluation, proximity_bonus;
    color winning_color, losing_color;
    white_evaluation = _count_material(board, COLOR_WHITE) + count_piece_square_value(board, COLOR_WHITE);
    black_evaluation = _count_material(board, COLOR_BLACK) + count_piece_square_value(board, COLOR_BLACK);
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
