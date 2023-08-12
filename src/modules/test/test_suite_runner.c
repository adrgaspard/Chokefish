#include "../serialization/consts.h"
#include "test_suite_runner.h"
#include "test_utils.h"

void run_node_exploration_test_suite()
{
    node_exploration_case init_pos_cases[] =
    { 
        { .depth = 0, .expected_nodes_count = 1 },
        { .depth = 1, .expected_nodes_count = 20 },
        { .depth = 2, .expected_nodes_count = 400 },
        { .depth = 3, .expected_nodes_count = 8902 },
        { .depth = 4, .expected_nodes_count = 197281 },
        { .depth = 5, .expected_nodes_count = 4865609 }
    };
    run_node_exploration_batch("INIT_POS", START_FEN_STR, init_pos_cases, 6);
}

void run_performance_tests_suite()
{

}