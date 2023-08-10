#ifndef SERIALIZATION_TYPES_H
#define SERIALIZATION_TYPES_H

#include "../core/types.h"

typedef struct move_data
{
    position start_pos;
    position dest_pos;
    promotion_type promotion_type;
} move_data;

typedef struct board_data
{
    piece position[POSITIONS_COUNT];
    color color_to_move;
    castling_data castling_data;
    int8_t last_en_passant_file;
    uint8_t half_move_count;
} board_data;

#endif // SERIALIZATION_TYPES_H
