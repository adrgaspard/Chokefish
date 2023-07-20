#include "precomputed_board_data.h"

#define WHITE_PAWN_OFFSET(y) y + 1
#define BLACK_PAWN_OFFSET(y) y - 1

typedef struct vector2
{
    int32_t x;
    int32_t y;
} vector2;

typedef struct index_validation_result
{
    bool valid;
    position index;
} index_validation_result;

static void initialize_positions_ranks_files_masks();
static void initialize_attacks_moves_masks();
static index_validation_result compute_index_if_valid(int32_t x, int32_t y);


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
    vector2 orthogonal_vectors[] = { 
        { -1, 0 }, 
        { 0, 1 }, 
        { 1, 0 }, 
        { 0, -1 } 
    };
    vector2 diagonal_vectors[] = { 
        { -1, -1 }, 
        { -1, 1 }, 
        { 1, 1 }, 
        { 1, -1 } 
    };
    vector2 knight_vectors[] = { 
        { -2, -1 }, 
        { -2, 1 }, 
        { -1, 2 }, 
        { 1, 2 }, 
        { 2, 1 }, 
        { 2, -1 }, 
        { 1, -2 }, 
        { -1, -2 } 
    };
    for (x = 0; x < FILES_COUNT; x++)
    {
        for (y = 0; y < RANKS_COUNT; y++)
        {
            pos_index = (position)(y * FILES_COUNT + x);
            // Compute king movements
            for (direction_index = 0; direction_index < 4; direction_index++)
            {           
                index_result = compute_index_if_valid(x + orthogonal_vectors[direction_index].x, y + orthogonal_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_king_moves[pos_index] |= 1UL << index_result.index;
                }
                index_result = compute_index_if_valid(x + diagonal_vectors[direction_index].x, y + diagonal_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_king_moves[pos_index] |= 1UL << index_result.index;
                }
            }
            // Compute knight attacks
            for (direction_index = 0; direction_index < 8; direction_index++)
            {
                index_result = compute_index_if_valid(x + knight_vectors[direction_index].x, y + knight_vectors[direction_index].y);
                if (index_result.valid)
                {
                    g_knight_attacks[pos_index] |= 1UL << index_result.index;
                }
            }
            // Compute pawn attacks
            index_result = compute_index_if_valid(x - 1, WHITE_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_WHITE][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x + 1, WHITE_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_WHITE][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x - 1, BLACK_PAWN_OFFSET(y));
            if (index_result.valid)
            {
                g_pawn_attacks[COLOR_BLACK][pos_index] |= 1UL << index_result.index;
            }
            index_result = compute_index_if_valid(x + 1, BLACK_PAWN_OFFSET(y));
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