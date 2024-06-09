#ifndef PIECE_GROUP_H
#define PIECE_GROUP_H

#include "types.h"

static inline bool is_piece_group_valid(piece_group *group);
static inline piece_group create_piece_group();
static inline void add_position_to_group(piece_group *group, position position);
static inline void remove_position_from_group(piece_group *group, position position);
static inline void move_position_in_group(piece_group *group, position start_pos, position dest_pos);

static inline bool is_piece_group_valid(piece_group *group)
{
    uint8_t i;
    if (group == NULL || group->count > PIECE_GROUP_MAX_SIZE)
    {
        return false;
    }
    for (i = 0; i < group->count; i++)
    {
        if (!is_position_valid(group->positions[i]) || group->positions[i] == NO_POSITION || group->map[group->positions[i]] != i)
        {
            return false;
        }
    }
    return true;
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
    assert(is_piece_group_valid(group));
    assert(position != NO_POSITION);
    group->positions[group->count] = position;
    group->map[position] = group->count;
    group->count++;
}

static inline void remove_position_from_group(piece_group *group, position position)
{
    uint8_t piece_index = group->map[position];
    assert(is_piece_group_valid(group));
    assert(position != NO_POSITION);
    group->positions[piece_index] = group->positions[group->count - 1];
    group->map[group->positions[piece_index]] = piece_index;
    group->count--;
}

static inline void move_position_in_group(piece_group *group, position start_pos, position dest_pos)
{
    uint8_t piece_index = group->map[start_pos];
    assert(is_piece_group_valid(group));
    assert(start_pos != NO_POSITION);
    assert(dest_pos != NO_POSITION);
    group->positions[piece_index] = dest_pos;
    group->map[dest_pos] = piece_index;
}

#endif // PIECE_GROUP_H
