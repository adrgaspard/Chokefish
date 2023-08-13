#include <stdint.h>
#include <stdio.h>
#include "../core/enhanced_time.h"
#include "../core/move_generation_result.h"
#include "../core/move_generator.h"
#include "../core/logging.h"
#include "../game_tools/game_data.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "test_utils.h"

#define INTO_BRACKET(content) FG_WHITE "[" COLOR_RESET content COLOR_RESET FG_WHITE "]" COLOR_RESET
#define PERFT_TEST_PREFIX INTO_BRACKET(FG_DARK_GREEN "Perft") " "
#define EXPLORATION_NODE_TEST_PREFIX INTO_BRACKET(FG_BLUE "Node exploration") " "
#define OK_RESULT BG_GREEN FG_WHITE "[OK]" COLOR_RESET
#define KO_RESULT BG_RED FG_WHITE "[KO]" COLOR_RESET

static move_generation_options s_move_generation_options =
{
    .include_quiet_moves = true,
    .promotion_types_to_include = PROMOTION_ALL
};

static game_data s_game_data;

static uint64_t search_nodes(board *board, int32_t depth);
static void run_node_exploration_test(char *fen_string, int32_t depth, uint64_t expected_nodes_count);

void run_node_exploration_batch(char *position_name, char *fen_string, node_exploration_case *cases, uint32_t cases_count)
{
    uint32_t case_index;
    node_exploration_case current_case;
    printf(EXPLORATION_NODE_TEST_PREFIX FG_GRAY "Starting batch " FG_YELLOW "%s" FG_GRAY ", with FEN = " FG_YELLOW "%s" FG_GRAY " ..." COLOR_RESET "\n",
        position_name, fen_string);
    for (case_index = 0; case_index < cases_count; case_index++)
    {
        current_case = cases[case_index];
        run_node_exploration_test(fen_string, current_case.depth, current_case.expected_nodes_count);
    }
}

uint64_t run_perft(char *fen_string, int32_t depth)
{
    move_generation_result move_generation_result;
    move current_move;
    uint32_t move_index;
    uint64_t total_nodes_count, current_move_nodes_count;
    board *board;
    reset_game_data(&(s_game_data), fen_string);
    board = &(s_game_data.board);
    char move_str[MOVE_DATA_STR_LEN];
    if (depth <= 0)
    {
        return depth == 0 ? 1ULL : 0;
    }
    move_generation_result = create_move_generation_result();
    generate_moves(board, &move_generation_result, s_move_generation_options);
    total_nodes_count = 0;
    printf(PERFT_TEST_PREFIX "Depth: %d\n\n", depth);
    for (move_index = 0; move_index < move_generation_result.moves_count; move_index++)
    {
        current_move = move_generation_result.moves[move_index];
        move_to_string(current_move, move_str);
        do_move(board, current_move, false);
        current_move_nodes_count = search_nodes(board, depth - 1);
        total_nodes_count += current_move_nodes_count;
        printf("%s: %lu\n", move_str, current_move_nodes_count);
        undo_move(board, current_move, false);
    }
    printf("\nNodes found: %lu\n\n", total_nodes_count);
    return total_nodes_count;
}

static uint64_t search_nodes(board *board, int32_t depth)
{
    move_generation_result move_generation_result;
    move current_move;
    uint32_t move_index;
    uint64_t nodes_count;
    if (depth <= 0)
    {
        return depth == 0 ? 1ULL : 0;
    }
    move_generation_result = create_move_generation_result();
    generate_moves(board, &move_generation_result, s_move_generation_options);
    nodes_count = 0;
    for (move_index = 0; move_index < move_generation_result.moves_count; move_index++)
    {
        current_move = move_generation_result.moves[move_index];
        do_move(board, current_move, false);
        nodes_count += search_nodes(board, depth - 1);
        undo_move(board, current_move, false);
    }
    return nodes_count;
}

static void run_node_exploration_test(char *fen_string, int32_t depth, uint64_t expected_nodes_count)
{
    uint64_t found_nodes_count, start_time, end_time;
    reset_game_data(&(s_game_data), fen_string);
    start_time = get_current_uptime();
    found_nodes_count = search_nodes(&(s_game_data.board), depth);
    end_time = get_current_uptime();
    printf("    ");
    if (expected_nodes_count == found_nodes_count)
    {
        printf(OK_RESULT);
    }
    else
    {
        printf(KO_RESULT);
    }
    printf(" " FG_GRAY "Depth: " FG_YELLOW "%u" FG_GRAY " / Expected nodes: " FG_YELLOW "%lu" FG_GRAY " / Found nodes: " FG_YELLOW "%lu"
        FG_GRAY " / Time elapsed: " FG_YELLOW "%.2f" COLOR_RESET "\n",
        depth, expected_nodes_count, found_nodes_count, (double)(end_time - start_time) / 1000.0);
}

