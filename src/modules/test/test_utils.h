#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../core/board.h"

typedef struct node_exploration_case
{
    int32_t depth;
    uint64_t expected_nodes_count;
} node_exploration_case;

void run_node_exploration_batch(char *position_name, char *fen_string, node_exploration_case *cases, uint32_t cases_count);

uint64_t run_perft(char *fen_string, int32_t depth);

#endif // TEST_UTILS_H
