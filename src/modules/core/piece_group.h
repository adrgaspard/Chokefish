#ifndef PIECE_GROUP_H
#define PIECE_GROUP_H

#include "types.h"

static inline bool is_piece_group_valid(piece_group *group);
static inline piece_group create_piece_group();
static inline void add_position_to_group(piece_group *group, position position);
static inline void remove_position_from_group(piece_group *group, position position);
static inline void move_position_in_group(piece_group *group, position position);

static inline bool is_piece_group_valid(piece_group *group)
{
    return group->count <= PIECE_GROUP_MAX_SIZE;
}

static inline piece_group create_piece_group()
{
    piece_group group;
    uint8_t i;
    for (i = 0; i < PIECE_GROUP_MAX_SIZE; i++)
    {
        group.positions[i] = 0;
    }
    for (i = 0; i < POSITIONS_COUNT; i++)
    {
        group.map[i] = 0;
    }
    group.count = 0;
    return group;
}

static inline void add_position_to_group(piece_group *group, position position)
{
    group->positions[group->count] = position;
    group->map[position] = group->count;
    group->count++;
}

static inline void remove_position_from_group(piece_group *group, position position)
{
    uint8_t piece_index = group->map[position];
    group->positions[piece_index] = group->positions[group->count - 1];
    group->map[group->positions[piece_index]] = piece_index;
    group->count--;
}

static inline void move_position_in_group(piece_group *group, position position)
{
    uint8_t piece_index = group->map[position];
    group->positions[piece_index] = position;
    group->map[position] = piece_index;
}

#endif // PIECE_GROUP_H
