#ifndef PIECE_VALUES_H
#define PIECE_VALUES_H

#include <stdint.h>
#include "../core/piece.h"

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 320
#define ROOK_VALUE 525
#define QUEEN_VALUE 950

int32_t get_piece_value(piece_type type);

#endif // PIECE_VALUES_H
