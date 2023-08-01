#include "bitboard.h"
#include "dynamic_collections.h"
#include "position.h"
#include "precomputed_board_data.h"
#include "static_collections.h"

#define _WHITE_PAWN_OFFSET(y) y + 1
#define _BLACK_PAWN_OFFSET(y) y - 1
#define _ORTHO_AND_DIAG_VECTORS_COUNT 4
#define _KNIGHT_VECTORS_COUNT 8

DECLARE_STATIC_ARRAY_TYPE(position, POSITIONS_COUNT, position_array64)
DECLARE_DYNAMIC_ARRAY_TYPE(bitboard, bitboard_dynamic_array)

typedef struct index_validation_result
{
    bool valid;
    position index;
} index_validation_result;

static vector2 s_orthogonal_vectors[] = { 
        { -1, 0 }, 
        { 0, 1 }, 
        { 1, 0 }, 
        { 0, -1 } 
    };
static vector2 s_diagonal_vectors[] = { 
        { -1, -1 }, 
        { -1, 1 }, 
        { 1, 1 }, 
        { 1, -1 } 
    };
static vector2 s_knight_vectors[] = { 
        { -2, -1 }, 
        { -2, 1 }, 
        { -1, 2 }, 
        { 1, 2 }, 
        { 2, 1 }, 
        { 2, -1 }, 
        { 1, -2 }, 
        { -1, -2 } 
    };

static void initialize_positions_ranks_files_masks();
static void initialize_attacks_moves_masks();
static index_validation_result compute_index_if_valid(int32_t x, int32_t y);
static bitboard_dynamic_array compute_blockers_bitboards(bitboard move_mask);
static bitboard compute_moves_mask(position start_pos, bool ortho_instead_of_diag);
static bitboard compute_legal_moves_mask(position start_pos, bitboard blockers_bitboard, bool ortho_instead_of_diag);

void initialize_precomputed_board_data()
{
    initialize_positions_ranks_files_masks();
    initialize_attacks_moves_masks();
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
            for (direction_index = 0; direction_index < _ORTHO_AND_DIAG_VECTORS_COUNT; direction_index++)
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
            for (direction_index = 0; direction_index < _KNIGHT_VECTORS_COUNT; direction_index++)
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

static index_validation_result compute_index_if_valid(int32_t x, int32_t y)
{
    index_validation_result result;
    result.valid = x >= 0 && x < FILES_COUNT && y >= 0 && y < RANKS_COUNT;
    result.index = result.valid ? (position)(y * FILES_COUNT + x) : NO_POSITION;
    return result;
}

static bitboard_dynamic_array compute_blockers_bitboards(bitboard move_mask)
{
    position_array64 indices;
    position pos;
    bitboard_dynamic_array combinations;
    uint32_t combination_index, bit, bit_index;
    indices = create_position_array64();
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        if (((move_mask >> pos) & 1) == 1)
        {
            append_on_position_array64(&indices, pos);
        }
    }
    combinations = create_bitboard_dynamic_array(1u << indices.count);
    for (combination_index = 0; combination_index < combinations.capacity; combination_index++)
    {
        append_on_bitboard_dynamic_array(&combinations, 0);
        for (bit_index = 0; bit_index < indices.count; bit_index++)
        {
            bit = (combination_index >> bit_index) & 1;
            combinations.items[combination_index] |= ((bitboard)bit) << indices.items[bit_index];
        }
    }
    return combinations;
}

static bitboard compute_moves_mask(position start_pos, bool ortho_instead_of_diag)
{
    bitboard mask;
    vector2 *vectors;
    vector2 start_pos_vector, current_pos_vector, next_pos_vector;
    int8_t vector_index, distance;
    index_validation_result pos_result;
    mask = 0;
    vectors = ortho_instead_of_diag ? s_orthogonal_vectors : s_diagonal_vectors;
    start_pos_vector = to_position_vector(start_pos);
    for (vector_index = 0; vector_index < _ORTHO_AND_DIAG_VECTORS_COUNT; vector_index++)
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

static bitboard compute_legal_moves_mask(position start_pos, bitboard blockers_bitboard, bool ortho_instead_of_diag)
{
    bitboard mask;
    vector2 *vectors;
    vector2 start_pos_vector, current_pos_vector;
    int8_t vector_index, distance;
    index_validation_result pos_result;
    mask = 0;
    vectors = ortho_instead_of_diag ? s_orthogonal_vectors : s_diagonal_vectors;
    start_pos_vector = to_position_vector(start_pos);
    for (vector_index = 0; vector_index < _ORTHO_AND_DIAG_VECTORS_COUNT; vector_index++)
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
                if (contains_position(blockers_bitboard, start_pos))
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
