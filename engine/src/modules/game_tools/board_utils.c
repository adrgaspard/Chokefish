#include "board_utils.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/types.h"

void reset_board(board *board, char *fen)
{
    board_data data;
    assert(board != NULL);
    assert(fen != NULL);
    *board = create_board();
    data = board_data_from_fen_string(fen);
    load_board_from_board_data(board, data);
}

void copy_board(board *source, board *destination)
{
    assert(source != NULL);
    assert(destination != NULL);
    *destination = *source;
}