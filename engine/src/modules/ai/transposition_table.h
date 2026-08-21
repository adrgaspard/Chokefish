#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>
#include "../core/types.h"
#include "search_utils.h"

// Must be a power of two: 2^24 entries x 16 bytes = 256 MB
#define TRANSPOSITION_TABLE_SIZE_IN_ENTRIES (1U << 24)

typedef enum node_type
{
    NODE_TYPE_NONE = 0,
    NODE_TYPE_EXACT = 1,
    NODE_TYPE_LOWER_BOUND = 2,
    NODE_TYPE_UPPER_BOUND = 3
} node_type;

typedef struct transposition_table_entry
{
    zobrist_key key;   // 8
    move best_move;    // 2
    int32_t score;     // 4
    uint8_t depth;     // 1
    uint8_t type;      // 1
} transposition_table_entry; // 16 bytes

void reset_transposition_table();
transposition_table_entry *get_transposition_table_entry(zobrist_key key);
void store_transposition_table_entry(zobrist_key key, uint16_t depth, node_type type, int32_t score, move best_move);

#endif // TRANSPOSITION_TABLE_H
