#include "piece_values.h"

int32_t get_piece_value(piece_type type)
{
    switch (type)
    {
        case PIECE_PAWN: return PAWN_VALUE;
        case PIECE_KNIGHT: return KNIGHT_VALUE;
        case PIECE_BISHOP: return BISHOP_VALUE;
        case PIECE_ROOK: return ROOK_VALUE;
        case PIECE_QUEEN: return QUEEN_VALUE;
        default: return 0;
    }
}
