#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "types.h"

void reset_game_data(game_data *data, char *fen_string);
void copy_game_data(game_data *destination, game_data *source);

#endif // GAME_DATA_H
