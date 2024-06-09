#ifndef GENERATION_TYPES_H
#define GENERATION_TYPES_H

#include "../core/types.h"

typedef struct magic_result
{
    bool valid;
    bitboard value;
    uint8_t bit_size;
} magic_result;

typedef struct position_generation_data
{
    bitboard_dynamic_array blockers_combinations;
    bitboard_dynamic_array bitboard_combinations;
} position_generation_data;

typedef struct magic_generation_data
{
    position_generation_data orthogonal_lookups[POSITIONS_COUNT];
    position_generation_data diagonal_lookups[POSITIONS_COUNT];
} magic_generation_data;

#endif // GENERATION_TYPES_H
