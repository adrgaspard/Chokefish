#include "precomputed_board_data.h"

static void initialize_positions_ranks_files_masks();

void initialize_precomputed_board_data()
{
    initialize_positions_ranks_files_masks();
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
    }
}
