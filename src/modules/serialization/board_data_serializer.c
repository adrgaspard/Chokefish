#include "board_data_serializer.h"

board_data board_data_from_fen_string(char *fen_string)
{
    board_data board_data;
    assert(fen_string);
    (void)fen_string;
    // TODO
    return board_data;
}

void board_data_to_fen_string(board_data board_data, char *result)
{
    assert(result);
    (void)board_data;
    (void)result;
    // TODO
}

void load_board_from_board_data(board *board, board_data board_data)
{
    assert(board);
    (void)board;
    (void)board_data;
    // TODO
}

board_data board_to_board_data(board *board)
{
    board_data board_data;
    assert(board);
    (void)board;
    // TODO
    return board_data;
}