#ifndef BOARD_DATA_SERIALIZER_H
#define BOARD_DATA_SERIALIZER_H

#include "types.h"

board_data board_data_from_fen_string(char *fen_string);
void board_data_to_fen_string(board_data board_data, char *result);
void load_board_from_board_data(board *board, board_data board_data);
board_data board_to_board_data(board *board);

#endif // BOARD_DATA_SERIALIZER_H
