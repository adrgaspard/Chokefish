#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "types.h"

void run_node_exploration_batch(char *position_name, char *fen_string, node_exploration_case *cases, uint32_t cases_count);

uint64_t run_performance_test(char *fen_string, int32_t depth);

#endif // TEST_UTILS_H
