#include "bitboard.h"
#include "position.h"
#include "precomputed_board_data.h"
#include "precomputed_magics.h"
#include "static_collections.h"

#define _WHITE_PAWN_OFFSET(y) (y + 1)
#define _BLACK_PAWN_OFFSET(y) (y - 1)
#define _CENTER_FIRST_HALF_VALUE (RANKS_COUNT / 2 - 1)
#define _CENTER_SECOND_HALF_VALUE (RANKS_COUNT / 2)

DECLARE_STATIC_ARRAY_CONTRACT(position, POSITIONS_COUNT, position_array64)
DECLARE_STATIC_ARRAY_IMPLEMENTATION(position, POSITIONS_COUNT, position_array64)

bitboard g_position_mask[POSITIONS_COUNT];
bitboard g_rank_mask[RANKS_COUNT];
bitboard g_file_mask[FILES_COUNT];
bitboard g_not_file_mask[FILES_COUNT];
bitboard g_knight_attacks[POSITIONS_COUNT];
bitboard g_pawn_attacks[PLAYERS_COUNT][POSITIONS_COUNT];
bitboard g_king_moves[POSITIONS_COUNT];
bitboard g_orthogonal_moves_mask[POSITIONS_COUNT];
bitboard g_diagonal_moves_mask[POSITIONS_COUNT];
magic_data g_orthogonal_magic_data[POSITIONS_COUNT];
magic_data g_diagonal_magic_data[POSITIONS_COUNT];
int8_t g_directional_offset[DIRECTIONAL_OFFSETS_COUNT];
uint8_t g_distances_to_edge[POSITIONS_COUNT][DIRECTIONAL_OFFSETS_COUNT];
uint8_t g_orthogonal_distance[POSITIONS_COUNT][POSITIONS_COUNT];
uint8_t g_center_manhattan_distance[POSITIONS_COUNT];
bitboard g_align_mask[POSITIONS_COUNT][POSITIONS_COUNT];
bitboard g_directional_ray_mask[DIRECTIONAL_OFFSETS_COUNT][POSITIONS_COUNT];

static vector2 s_directional_vectors[] =
{ 
    { 0, 1 }, 
    { 0, -1 }, 
    { -1, 0 }, 
    { 1, 0 },
    { -1, 1 }, 
    { 1, -1 }, 
    { 1, 1 }, 
    { -1, -1 } 
};

static vector2 s_orthogonal_vectors[] =
{ 
    { 0, 1 }, 
    { 0, -1 }, 
    { -1, 0 }, 
    { 1, 0 } 
};

static vector2 s_diagonal_vectors[] =
{ 
    { -1, 1 }, 
    { 1, -1 }, 
    { 1, 1 }, 
    { -1, -1 } 
};

static vector2 s_knight_vectors[] =
{ 
    { -2, -1 }, 
    { -2, 1 }, 
    { -1, 2 }, 
    { 1, 2 }, 
    { 2, 1 }, 
    { 2, -1 }, 
    { 1, -2 }, 
    { -1, -2 } 
};

DECLARE_MAGIC_VALUES()

static void initialize_directional_offsets();
static void initialize_distances();
static void initialize_align_masks();
static void initialize_directional_ray_masks();
static void initialize_directional_moves_masks_and_magics();
static void initialize_positions_ranks_files_masks();
static void initialize_attacks_moves_masks();
static void initialize_magic_data(position position, bool ortho_instead_of_diag);
static position_array64 compute_blockers_indices(bitboard moves_mask, uint32_t *indices_count);
static void compute_blockers_combinations(bitboard *combinations, uint32_t combinations_capacity, position_array64 indices, uint32_t indices_count);

void initialize_precomputed_board_data()
{
    initialize_directional_offsets();
    initialize_distances();
    initialize_align_masks();
    initialize_directional_ray_masks();
    initialize_directional_moves_masks_and_magics();
    initialize_positions_ranks_files_masks();
    initialize_attacks_moves_masks();
}

bitboard_dynamic_array compute_blockers_bitboards(bitboard moves_mask)
{
    position_array64 indices;
    bitboard_dynamic_array combinations;
    uint32_t indices_count;
    indices = compute_blockers_indices(moves_mask, &indices_count);
    combinations = create_bitboard_dynamic_array(1U << indices_count);
    compute_blockers_combinations(combinations.items, combinations.capacity, indices, indices_count);
    return combinations;
}

bitboard compute_moves_mask(position start_pos, bool ortho_instead_of_diag)
{
    bitboard mask;
    vector2 *vectors;
    vector2 start_pos_vector, current_pos_vector, next_pos_vector;
    int8_t vector_index, distance;
    index_validation_result pos_result;
    assert(is_position_valid(start_pos) && start_pos != NO_POSITION);
    mask = 0;
    vectors = ortho_instead_of_diag ? s_orthogonal_vectors : s_diagonal_vectors;
    start_pos_vector = to_position_vector(start_pos);
    for (vector_index = 0; vector_index < ORTHO_OR_DIAG_OFFSETS_COUNT; vector_index++)
    {
        for (distance = 1; distance < RANKS_COUNT; distance++)
        {
            current_pos_vector = start_pos_vector;
            next_pos_vector = start_pos_vector;
            current_pos_vector.x += vectors[vector_index].x * distance;
            current_pos_vector.y += vectors[vector_index].y * distance;
            next_pos_vector.x += vectors[vector_index].x * (distance + 1);
            next_pos_vector.y += vectors[vector_index].y * (distance + 1);
            pos_result = compute_index_if_valid(next_pos_vector.x, next_pos_vector.y);
            if (pos_result.valid)
            {
                set_position_to_one(&mask, compute_index_if_valid(current_pos_vector.x, current_pos_vector.y).index);
            }
            else
            {
                break;
            }
        }
    }
    return mask;
}

bitboard compute_legal_moves_mask(position start_pos, bitboard blockers_bitboard, bool ortho_instead_of_diag)
{
    bitboard mask;
    vector2 *vectors;
    vector2 start_pos_vector, current_pos_vector;
    int8_t vector_index, distance;
    index_validation_result pos_result;
    assert(is_position_valid(start_pos) && start_pos != NO_POSITION);
    mask = 0;
    vectors = ortho_instead_of_diag ? s_orthogonal_vectors : s_diagonal_vectors;
    start_pos_vector = to_position_vector(start_pos);
    for (vector_index = 0; vector_index < ORTHO_OR_DIAG_OFFSETS_COUNT; vector_index++)
    {
        for (distance = 1; distance < RANKS_COUNT; distance++)
        {
            current_pos_vector = start_pos_vector;
            current_pos_vector.x += vectors[vector_index].x * distance;
            current_pos_vector.y += vectors[vector_index].y * distance;
            pos_result = compute_index_if_valid(current_pos_vector.x, current_pos_vector.y);
            if (pos_result.valid)
            {
                set_position_to_one(&mask, pos_result.index);
                if (contains_position(blockers_bitboard, pos_result.index))
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    return mask;
}

bitboard get_slider_moves_bitboard(position position, bitboard blockers, bool ortho_instead_of_diag)
{
    assert(is_position_valid(position) && position != NO_POSITION);
    return ortho_instead_of_diag ? get_orthogonal_moves_bitboard(position, blockers) : get_diagonal_moves_bitboard(position, blockers);
}

bitboard get_orthogonal_moves_bitboard(position position, bitboard blockers)
{
    bitboard key;
    magic_data *data;
    assert(is_position_valid(position) && position != NO_POSITION);
    data = &(g_orthogonal_magic_data[position]);
    key = ((blockers & g_orthogonal_moves_mask[position]) * data->value) >> data->shift_quantity;
    return data->legal_moves[key];
}

bitboard get_diagonal_moves_bitboard(position position, bitboard blockers)
{
    bitboard key;
    magic_data *data;
    assert(is_position_valid(position) && position != NO_POSITION);
    data = &(g_diagonal_magic_data[position]);
    key = ((blockers & g_diagonal_moves_mask[position]) * data->value) >> data->shift_quantity;
    return data->legal_moves[key];
}

static void initialize_directional_offsets()
{
    g_directional_offset[0] = FILES_COUNT;
    g_directional_offset[1] = -FILES_COUNT;
    g_directional_offset[2] = -1;
    g_directional_offset[3] = 1;
    g_directional_offset[4] = FILES_COUNT - 1;
    g_directional_offset[5] = -FILES_COUNT + 1;
    g_directional_offset[6] = FILES_COUNT + 1;
    g_directional_offset[7] = -FILES_COUNT - 1;
}

static void initialize_distances()
{
    position start_pos, dest_pos;
    vector2 start_pos_vector, dest_pos_vector;
    int8_t top, bottom, left, right, tmp_a, tmp_b, file_distance, rank_distance, file_distance_from_center, rank_distance_from_center;
    for (start_pos = 0; start_pos < POSITIONS_COUNT; start_pos++)
    {
        start_pos_vector = to_position_vector(start_pos);
        top = (int8_t)(FILES_COUNT - 1 - start_pos_vector.y);
        bottom = (int8_t)start_pos_vector.y;
        left = (int8_t)start_pos_vector.x;
        right = (int8_t)(RANKS_COUNT - 1 - start_pos_vector.x);
        g_distances_to_edge[start_pos][0] = (uint8_t)top;
        g_distances_to_edge[start_pos][1] = (uint8_t)bottom;
        g_distances_to_edge[start_pos][2] = (uint8_t)left;
        g_distances_to_edge[start_pos][3] = (uint8_t)right;
        g_distances_to_edge[start_pos][4] = (uint8_t)(top < left ? top : left);
        g_distances_to_edge[start_pos][5] = (uint8_t)(bottom < right ? bottom : right);
        g_distances_to_edge[start_pos][6] = (uint8_t)(top < right ? top : right);
        g_distances_to_edge[start_pos][7] = (uint8_t)(bottom < left ? bottom : left);
        tmp_a = (int8_t)(_CENTER_FIRST_HALF_VALUE - start_pos_vector.x);
        tmp_b = (int8_t)(start_pos_vector.x - _CENTER_SECOND_HALF_VALUE);
        file_distance_from_center = tmp_a > tmp_b ? tmp_a : tmp_b;
        tmp_a = (int8_t)(_CENTER_FIRST_HALF_VALUE - start_pos_vector.y);
        tmp_b = (int8_t)(start_pos_vector.y - _CENTER_SECOND_HALF_VALUE);
        rank_distance_from_center = tmp_a > tmp_b ? tmp_a : tmp_b;
        g_center_manhattan_distance[start_pos] = (uint8_t)(file_distance_from_center + rank_distance_from_center);
        for (dest_pos = 0; dest_pos < POSITIONS_COUNT; dest_pos++)
        {
            dest_pos_vector = to_position_vector(dest_pos);
            file_distance = (int8_t)(start_pos_vector.x - dest_pos_vector.x);
            file_distance = (int8_t)(file_distance >= 0 ? file_distance : -file_distance);
            rank_distance = (int8_t)(start_pos_vector.y - dest_pos_vector.y);
            rank_distance = (int8_t)(rank_distance >= 0 ? rank_distance : -rank_distance);
            g_orthogonal_distance[start_pos][dest_pos] = (uint8_t)(file_distance + rank_distance);
        }
    }
}

static void initialize_align_masks()
{
    int8_t i;
    position start_pos, dest_pos;
    vector2 start_pos_vector, dest_pos_vector, offset_vector, direction_vector, align_vector;
    index_validation_result index_result;
    for (start_pos = 0; start_pos < POSITIONS_COUNT; start_pos++)
    {
        for (dest_pos = 0; dest_pos < POSITIONS_COUNT; dest_pos++)
        {
            g_align_mask[start_pos][dest_pos] = 0ULL;
            start_pos_vector = to_position_vector(start_pos);
            dest_pos_vector = to_position_vector(dest_pos);
            offset_vector.x = dest_pos_vector.x - start_pos_vector.x;
            offset_vector.y = dest_pos_vector.y - start_pos_vector.y;
            direction_vector.x = offset_vector.x == 0 ? 0 : (offset_vector.x > 0 ? 1 : -1);
            direction_vector.y = offset_vector.y == 0 ? 0 : (offset_vector.y > 0 ? 1 : -1);
            for (i = -FILES_COUNT; i < FILES_COUNT; i++)
            {
                align_vector.x = start_pos_vector.x + direction_vector.x * i;
                align_vector.y = start_pos_vector.y + direction_vector.y * i;
                index_result = compute_index_if_valid(align_vector.x, align_vector.y);
                if (index_result.valid)
                {
                    g_align_mask[start_pos][dest_pos] |= 1ULL << index_result.index;
                }
            }
        }
    }
}

static void initialize_directional_ray_masks()
{
    int8_t i;
    uint8_t direction_index;
    position pos;
    vector2 pos_vector, ray_vector;
    index_validation_result index_result;
    for (direction_index = 0; direction_index < DIRECTIONAL_OFFSETS_COUNT; direction_index++)
    {
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            g_directional_ray_mask[direction_index][pos] = 0ULL;
            pos_vector = to_position_vector(pos);
            for (i = 0; i < FILES_COUNT; i++)
            {
                ray_vector.x = pos_vector.x + s_directional_vectors[direction_index].x * i;
                ray_vector.y = pos_vector.y + s_directional_vectors[direction_index].y * i;
                index_result = compute_index_if_valid(ray_vector.x, ray_vector.y);
                if (index_result.valid)
                {
                    g_directional_ray_mask[direction_index][pos] |= 1ULL << index_result.index;
                }
                else
                {
                    break;
                }
            }
        }
    }
}

static void initialize_directional_moves_masks_and_magics()
{
    position pos;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        g_orthogonal_moves_mask[pos] = compute_moves_mask(pos, true);
        g_diagonal_moves_mask[pos] = compute_moves_mask(pos, false);
    }
    ASSIGN_MAGIC_VALUES()
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        initialize_magic_data(pos, true);
        initialize_magic_data(pos, false);
    }
}

static void initialize_positions_ranks_files_masks()
{
    position position;
    uint8_t rank, file;
    for (position = 0; position < POSITIONS_COUNT; position++)
    {
        g_position_mask[position] = 1ULL << position;
    }
    for (rank = 0; rank < RANKS_COUNT; rank++)
    {
        g_rank_mask[rank] = 0xFFULL << (FILES_COUNT * rank);
    }
    for (file = 0; file < FILES_COUNT; file++)
    {
        g_file_mask[file] = 0x0101010101010101ULL << file;
        g_not_file_mask[file] = ~g_file_mask[file];
    }
}

static void initialize_attacks_moves_masks()
{
    int32_t x, y, direction_index;
    index_validation_result index_result;
    position pos_index;
    for (x = 0; x < FILES_COUNT; x++)
    {
        for (y = 0; y < RANKS_COUNT; y++)
        {
            pos_index = (position)(y * FILES_COUNT + x);
            // Compute king movements
            for (direction_index = 0; direction_index < ORTHO_OR_DIAG_OFFSETS_COUNT; direction_index++)
            {           
                index_result = compute_index_if_valid(x + s_orthogonal_vectors[direction_index].x, y + s_orthogonal_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_king_moves[pos_index] |= 1UL << index_result.index;
                }
                index_result = compute_index_if_valid(x + s_diagonal_vectors[direction_index].x, y + s_diagonal_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_king_moves[pos_index] |= 1UL << index_result.index;
                }
            }
            // Compute knight attacks
            for (direction_index = 0; direction_index < KNIGHT_OFFSETS_COUNT; direction_index++)
            {
                index_result = compute_index_if_valid(x + s_knight_vectors[direction_index].x, y + s_knight_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_knight_attacks[pos_index] |= 1UL << index_result.index;
                }
            }
            // Compute pawn attacks
            index_result = compute_index_if_valid(x - 1, _WHITE_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_WHITE][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x + 1, _WHITE_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_WHITE][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x - 1, _BLACK_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_BLACK][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x + 1, _BLACK_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_BLACK][pos_index] |= 1UL << index_result.index;
            }
        }
    }
}

static void initialize_magic_data(position position, bool ortho_instead_of_diag)
{
    magic_data *magic;
    bitboard moves_mask, legal_moves_mask, key;
    bitboard_dynamic_array blockers_combinations;
    uint32_t combination_index;
    assert(is_position_valid(position) && position != NO_POSITION);
    magic = ortho_instead_of_diag ? &(g_orthogonal_magic_data[position]) : &(g_diagonal_magic_data[position]);
    moves_mask = ortho_instead_of_diag ? g_orthogonal_moves_mask[position] : g_diagonal_moves_mask[position];
    blockers_combinations = compute_blockers_bitboards(moves_mask);
    for (combination_index = 0; combination_index < blockers_combinations.capacity; combination_index++)
    {
        key = (blockers_combinations.items[combination_index] * magic->value) >> magic->shift_quantity;
        legal_moves_mask = compute_legal_moves_mask(position, blockers_combinations.items[combination_index], ortho_instead_of_diag);
        magic->legal_moves[key] = legal_moves_mask;
    }
    finalize_bitboard_dynamic_array(&blockers_combinations);
}

static position_array64 compute_blockers_indices(bitboard moves_mask, uint32_t *indices_count)
{
    position_array64 indices;
    position pos;
    uint32_t count;
    indices = create_position_array64();
    count = 0;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        if (((moves_mask >> pos) & 1) == 1)
        {
            indices.items[count] = pos;
            count++;
        }
    }
    if (indices_count != NULL)
    {
        *indices_count = count;
    }
    return indices;
}

static void compute_blockers_combinations(bitboard *combinations, uint32_t combinations_capacity, position_array64 indices, uint32_t indices_count)
{
    uint32_t combination_index, bit, bit_index;
    assert(combinations != NULL);
    for (combination_index = 0; combination_index < combinations_capacity; combination_index++)
    {
        combinations[combination_index] = 0;
        for (bit_index = 0; bit_index < indices_count; bit_index++)
        {
            bit = (combination_index >> bit_index) & 1;
            assert(combinations_capacity > combination_index);
            combinations[combination_index] |= ((bitboard)bit) << indices.items[bit_index];
        }
    }
}
