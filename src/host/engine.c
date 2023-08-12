#include <string.h>
#include "../modules/core/board.h"
#include "../modules/core/move_generator.h"
#include "../modules/core/precomputed_board_data.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"
#include "../modules/serialization/board_data_serializer.h"
#include "../modules/serialization/consts.h"

int main()
{
    char result_fen[200];
    move_stack move_stack;
    zobrist_stack zobrist_stack;
    game_state_stack game_state_stack;
    board board;
    initialize_shared_random(42);
    initialize_zobrist();
    initialize_precomputed_board_data();
    move_stack = create_move_stack();
    zobrist_stack = create_zobrist_stack();
    game_state_stack = create_game_state_stack();
    board = create_board(&game_state_stack, &move_stack, &zobrist_stack);
    load_board_from_board_data(&board, board_data_from_fen_string(START_FEN_STR));
    strcpy(result_fen, "Invalid FEN");
    board_to_fen_string(&board, result_fen);
    printf("Current FEN: %s\n", result_fen);
    return 0;
}