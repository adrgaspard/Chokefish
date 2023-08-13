#include "game_data.h"
#include "../core/board.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/types.h"

void reset_game_data(game_data *data, char *fen_string)
{
    board_data board_data;
    assert(data != NULL);
    data->move_stack = create_move_stack();
    data->zobrist_stack = create_zobrist_stack();
    data->game_state_stack = create_game_state_stack();
    data->board = create_board(&(data->game_state_stack), &(data->move_stack), &(data->zobrist_stack));
    if (fen_string != NULL)
    {
        board_data = board_data_from_fen_string(fen_string);
        load_board_from_board_data(&(data->board), board_data);
    }
}