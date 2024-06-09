#include <assert.h>
#include "castling.h"
#include "game_state.h"
#include "piece.h"

bool is_game_state_valid(game_state game_state)
{
    return is_castling_data_valid(game_state.castling_data) && is_piece_valid(game_state.captured_piece)
        && ((game_state.last_en_passant_file < FILES_COUNT && game_state.last_en_passant_file >= 0) || game_state.last_en_passant_file == NO_FILE);
}

game_state create_game_state(castling_data castling_data, int8_t last_en_passant_file, uint8_t silent_move_count, piece captured_piece, zobrist_key key)
{
    game_state state;
    assert(is_castling_data_valid(castling_data));
    assert(last_en_passant_file == NO_FILE || (last_en_passant_file >= 0 && last_en_passant_file < FILES_COUNT));
    assert(silent_move_count <= PASSIVE_HALF_MOVE_COUNT_MAX);
    assert(is_piece_valid(captured_piece));
    state.castling_data = castling_data;
    state.last_en_passant_file = last_en_passant_file;
    state.silent_move_count = silent_move_count;
    state.captured_piece = captured_piece;
    state.zobrist_key = key;
    return state;
}
