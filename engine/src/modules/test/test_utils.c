#include <stdint.h>
#include <stdio.h>
#include "../core/enhanced_time.h"
#include "../core/move_generation_result.h"
#include "../core/move_generator.h"
#include "../core/logging.h"
#include "../game_tools/board_utils.h"
#include "../serialization/board_data_serializer.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "test_utils.h"

#define INTO_BRACKET(content) FG_WHITE "[" COLOR_RESET content COLOR_RESET FG_WHITE "]" COLOR_RESET
#define PERFT_TEST_PREFIX INTO_BRACKET(FG_DARK_GREEN "Performance") " "
#define EXPLORATION_NODE_TEST_PREFIX INTO_BRACKET(FG_BLUE "Node exploration") " "
#define OK_RESULT BG_GREEN FG_WHITE "[OK]" COLOR_RESET
#define KO_RESULT BG_RED FG_WHITE "[KO]" COLOR_RESET

static move_generation_options s_move_generation_options =
{
    .include_quiet_moves = true,
    .promotion_types_to_include = PROMOTION_ALL
};

static board s_board;

static void run_node_exploration_case(char *fen_string, int32_t depth, uint64_t expected_nodes_count);
static uint64_t search_nodes(board *board, int32_t depth);

void run_node_exploration_batch(char *position_name, char *fen_string, node_exploration_case *cases, uint32_t cases_count)
{
    uint32_t case_index;
    node_exploration_case current_case;
    assert(position_name != NULL);
    assert(fen_string != NULL);
    assert(cases != NULL);
    printf(EXPLORATION_NODE_TEST_PREFIX FG_GRAY "Starting batch " FG_CYAN "%s" FG_GRAY ", with FEN = " FG_CYAN "%s" FG_GRAY " ..." COLOR_RESET "\n",
        position_name, fen_string);
    fflush(stdout);
    for (case_index = 0; case_index < cases_count; case_index++)
    {
        current_case = cases[case_index];
        run_node_exploration_case(fen_string, current_case.depth, current_case.expected_nodes_count);
    }
}

uint64_t run_performance_test(char *fen_string, int32_t depth)
{
    move_generation_result move_generation_result;
    move current_move;
    uint32_t move_index;
    uint64_t total_nodes_count, current_move_nodes_count, start_time, total_uptime;
    char move_str[MOVE_DATA_STR_LEN];
    assert(fen_string != NULL);
    reset_board(&s_board, fen_string);
    total_nodes_count = 0;
    total_uptime = 0;
    if (depth <= 0)
    {
        return depth == 0 ? 1ULL : 0;
    }
    move_generation_result = create_move_generation_result();    
    generate_moves(&s_board, &move_generation_result, s_move_generation_options);
    printf(PERFT_TEST_PREFIX FG_GRAY "Starting with a depth of " FG_CYAN I32 FG_GRAY ", with FEN = " FG_CYAN "%s" FG_GRAY " ..."
        COLOR_RESET "\n", depth, fen_string);
    fflush(stdout);
    for (move_index = 0; move_index < move_generation_result.moves_count; move_index++)
    {
        current_move = move_generation_result.moves[move_index];
        move_to_string(current_move, move_str);
        start_time = get_current_uptime();
        do_move(&s_board, current_move, false);
        current_move_nodes_count = search_nodes(&s_board, depth - 1);
        undo_move(&s_board, current_move, false);
        total_uptime += get_current_uptime() - start_time;
        total_nodes_count += current_move_nodes_count;
        printf(FG_GRAY "    - %s: " FG_YELLOW U64 COLOR_RESET "\n", move_str, current_move_nodes_count);
        fflush(stdout);
    }
    printf(FG_GRAY"    Total nodes found: " FG_YELLOW U64 COLOR_RESET "\n", total_nodes_count);
    printf(FG_GRAY"    Time elapsed: " FG_YELLOW "%.2fs" COLOR_RESET "\n", (double)(total_uptime) / 1000.0);
    fflush(stdout);
    return total_nodes_count;
}

static void run_node_exploration_case(char *fen_string, int32_t depth, uint64_t expected_nodes_count)
{
    uint64_t found_nodes_count, start_time, end_time;
    assert(fen_string != NULL);
    reset_board(&s_board, fen_string);
    start_time = get_current_uptime();
    found_nodes_count = search_nodes(&s_board, depth);
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
    printf(" " FG_GRAY "Depth: " FG_YELLOW I32 FG_GRAY " / Expected nodes: " FG_YELLOW U64 FG_GRAY " / Found nodes: " FG_YELLOW U64
        FG_GRAY " / Time elapsed: " FG_YELLOW "%.2fs" COLOR_RESET "\n",
        depth, expected_nodes_count, found_nodes_count, (double)(end_time - start_time) / 1000.0);
    fflush(stdout);
}

static uint64_t search_nodes(board *board, int32_t depth)
{
    move_generation_result move_generation_result;
    move current_move;
    uint32_t move_index;
    uint64_t nodes_count;
    assert(board != NULL);
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
