#ifndef ORDERING_V2_H
#define ORDERING_V2_H

#include "../core/move_generator.h"
#include "../core/move_generation_result.h"
#include "../core/types.h"

#define PRIORITY_MOVE_BONUS 10000
#define ATTACKED_SQUARE_PENALTY 350

void sort_moves_v2(board *board, move_generation_data *data, move_generation_result *result, move priority_move);

#endif // ORDERING_V2_H
