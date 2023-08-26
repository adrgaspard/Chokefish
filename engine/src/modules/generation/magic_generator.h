#ifndef MAGIC_GENERATOR_H
#define MAGIC_GENERATOR_H

#include "../core/precomputed_board_data.h"
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

magic_generation_data create_magic_generation_data();
magic_result compute_magic(magic_generation_data *data, position start_pos, bool ortho_instead_of_diag, uint32_t iterations_count, uint8_t max_bit_size);
bool is_new_magic_result_better(magic_result old_result, magic_result new_result);

#endif // MAGIC_GENERATOR_H
