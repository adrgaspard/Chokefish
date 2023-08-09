#ifndef MOVE_DATA_SERIALIZER_H
#define MOVE_DATA_SERIALIZER_H

#include "../core/consts.h"
#include "types.h"

void move_data_to_string(move_data move_data, char *result);
move_data move_data_from_string(char *string);
move move_data_to_existing_move(move_data move_data, move *existing_moves, uint8_t existing_moves_count);

#endif // MOVE_DATA_SERIALIZER_H
