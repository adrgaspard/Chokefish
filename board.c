#include "board.h"

void initialize_board_precomputed_data()
{
    position position;
    uint8_t rank, file;
    for (position = 0; position < POSITIONS_COUNT; position++) {
        g_position_mask[position] = 1ULL << position;
    }
    for (rank = 0; rank < RANKS_COUNT; rank++) {
        g_rank_mask[rank] = 0xFFULL << (FILES_COUNT * rank);
    }
    for (file = 0; file < FILES_COUNT; file++) {
        g_file_mask[file] = 0x0101010101010101ULL << file;
    }
}

