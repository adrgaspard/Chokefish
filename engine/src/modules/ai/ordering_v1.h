#ifndef ORDERING_V1_H
#define ORDERING_V1_H

#include "../core/move_generator.h"
#include "../core/move_generation_result.h"
#include "../core/types.h"

#define PRIORITY_MOVE_BONUS 10000

void sort_moves_v1(board *board, move_generation_data *data, move_generation_result *result, move priority_move);

#endif // ORDERING_V1_H
