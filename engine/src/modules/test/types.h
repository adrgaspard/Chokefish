#ifndef TEST_TYPES_H
#define TEST_TYPES_H

#include "../core/types.h"

typedef struct node_exploration_case
{
    int32_t depth;
    uint64_t expected_nodes_count;
} node_exploration_case;

#endif // TEST_TYPES_H
