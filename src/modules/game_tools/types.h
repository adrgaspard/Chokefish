#ifndef GAME_TOOLS_TYPES_H
#define GAME_TOOLS_TYPES_H

#include "../core/types.h"

typedef struct game_data
{
    move_stack move_stack;
    zobrist_stack zobrist_stack;
    game_state_stack game_state_stack;
    board board;
} game_data;

#endif // GAME_TOOLS_TYPES_H
