#ifndef GAME_TOOLS_TYPES_H
#define GAME_TOOLS_TYPES_H

#include "../core/types.h"

typedef enum time_system
{
    TS_NONE = 0,
    TS_INFINITE = 1,
    TS_DEFINED = 2,
    TS_INCREMENTAL = 3,
    TS_CONTROL = 4
} time_system;

typedef struct game_data
{
    move_stack move_stack;
    zobrist_stack zobrist_stack;
    game_state_stack game_state_stack;
    board board;
} game_data;

#endif // GAME_TOOLS_TYPES_H
