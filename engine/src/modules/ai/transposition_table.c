#include <string.h>
#include "transposition_table.h"

static transposition_table_entry s_transposition_table[TRANSPOSITION_TABLE_SIZE_IN_ENTRIES];
static const uint32_t s_transposition_table_index_mask = TRANSPOSITION_TABLE_SIZE_IN_ENTRIES - 1;

void reset_transposition_table()
{
    memset(s_transposition_table, 0, sizeof(s_transposition_table));
}

transposition_table_entry *get_transposition_table_entry(zobrist_key key)
{
    return &(s_transposition_table[key & s_transposition_table_index_mask]);
}

void store_transposition_table_entry(zobrist_key key, uint16_t depth, node_type type, int32_t score, move best_move)
{
    transposition_table_entry *entry = get_transposition_table_entry(key);
    if (entry->key != key || depth >= entry->depth)
    {
        entry->key = key;
        entry->depth = (uint8_t)depth;
        entry->type = (uint8_t)type;
        entry->score = score;
        entry->best_move = best_move;
    }
}
