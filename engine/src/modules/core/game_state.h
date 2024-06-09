#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"

bool is_game_state_valid(game_state game_state);
game_state create_game_state(castling_data castling_data, int8_t last_en_passant_file, uint8_t silent_move_count, piece captured_piece, zobrist_key key);

#endif // GAME_STATE_H
