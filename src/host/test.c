#include "../modules/core/board.h"
#include "../modules/core/shared_random.h"
#include "../modules/core/zobrist.h"
#include "../modules/test/test_suite_runner.h"

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
    run_node_exploration_test_suite();
    return 0;
}