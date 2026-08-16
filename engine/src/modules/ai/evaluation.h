#ifndef EVALUATION_H
#define EVALUATION_H

#include "../core/types.h"

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 320
#define ROOK_VALUE 525
#define QUEEN_VALUE 950

int32_t evaluation_v0(board *board);
int32_t _count_material(board *board, color player);

#endif // EVALUATION_H
