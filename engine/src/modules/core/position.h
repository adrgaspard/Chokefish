#ifndef POSITION_H
#define POSITION_H

#include "types.h"

bool is_position_valid(position position);
int8_t get_file(position position);
int8_t get_rank(position position);
vector2 to_position_vector(position position);
index_validation_result compute_index_if_valid(int32_t x, int32_t y);

#endif // POSITION_H