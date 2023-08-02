#include "../core/shared_random.h"
#include "magic_generator.h"

static position_generation_data compute_position_generation_data(position start_pos, bool ortho_instead_of_diag);
static bool is_magic_valid(position_generation_data *lookup, bitboard_dynamic_array test, bitboard magic, uint8_t bit_size);

magic_generation_data create_magic_generation_data()
{
    magic_generation_data data;
    position pos;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        data.orthogonal_lookups[pos] = compute_position_generation_data(pos, true);
        data.diagonal_lookups[pos] = compute_position_generation_data(pos, false);
    }
    return data;
}

magic_result compute_magic(magic_generation_data *data, position start_pos, bool ortho_instead_of_diag, uint32_t iterations_count, uint8_t max_bit_size)
{
    magic_result result;
    bitboard_dynamic_array test;
    bitboard best_magic, current_magic;
    position_generation_data lookup;
    uint8_t min_size_found, current_size;
    uint32_t entries_count, i, j;
    lookup = ortho_instead_of_diag ? data->orthogonal_lookups[start_pos] : data->diagonal_lookups[start_pos];
    best_magic = 0;
    min_size_found = (uint8_t)(max_bit_size + 1);
    entries_count = (uint32_t)1 << max_bit_size;
    test = create_bitboard_dynamic_array(entries_count);
    for (i = 0; i < iterations_count; i++)
    {
        current_magic = (bitboard)get_rand_u64();
        for (current_size = (uint8_t)(min_size_found - 1); current_size > 0; current_size--)
        {
            clear_bitboard_dynamic_array(&test);
            for (j = 0; j < (uint32_t)(1 << (uint8_t)(min_size_found - 1)); j++)
            {
                append_on_bitboard_dynamic_array(&test, 0);
            }
            if (is_magic_valid(&lookup, test, current_magic, current_size))
            {
                best_magic = current_magic;
                min_size_found = current_size;
            }
        }
    }
    finalize_bitboard_dynamic_array(&test);
    result.valid = best_magic != 0;
    result.value = best_magic;
    result.bit_size = min_size_found;
    return result;
}

bool is_new_magic_result_better(magic_result old_result, magic_result new_result)
{
    return new_result.valid && (!old_result.valid || old_result.bit_size > new_result.bit_size);
}

static position_generation_data compute_position_generation_data(position start_pos, bool ortho_instead_of_diag)
{
    position_generation_data data;
    bitboard moves_mask;
    bitboard_dynamic_array blocker_bitboards, legal_moves_bitboards;
    uint32_t bitboard_index;
    moves_mask = compute_moves_mask(start_pos, ortho_instead_of_diag);
    printf("Move mask provided: %lu\n", moves_mask);
    blocker_bitboards = compute_blockers_bitboards(moves_mask);
    legal_moves_bitboards = create_bitboard_dynamic_array(blocker_bitboards.capacity);
    for (bitboard_index = 0; bitboard_index < blocker_bitboards.count; bitboard_index++)
    {
        append_on_bitboard_dynamic_array(&legal_moves_bitboards, compute_legal_moves_mask(start_pos, blocker_bitboards.items[bitboard_index], ortho_instead_of_diag));
    }
    data.blockers_combinations = blocker_bitboards;
    data.bitboard_combinations = legal_moves_bitboards;
    return data;
}

static bool is_magic_valid(position_generation_data *lookup, bitboard_dynamic_array test, bitboard magic, uint8_t bit_size)
{
    uint32_t bitboard_index, key;
    bitboard blockers, moves;

    for (bitboard_index = 0; bitboard_index < lookup->blockers_combinations.count; bitboard_index++)
    {
        blockers = lookup->blockers_combinations.items[bitboard_index];
        moves = lookup->bitboard_combinations.items[bitboard_index];
        key = (uint32_t)((blockers * magic) >> (POSITIONS_COUNT - bit_size));
        if (test.items[key] == 0 || test.items[key] == moves)
        {
            test.items[key] = moves;
        }
        else 
        {
            return false;
        }
    }
    //printf("New magic detected with %u bits and with %u blockers combinations\n", bit_size, lookup->blockers_combinations.count);
    return true;
}
