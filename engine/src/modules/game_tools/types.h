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

typedef enum game_result
{
    GR_NONE = 0x00,
    GR_PLAYING = 0x01,
    GR_WHITE_IS_MATED = 0x02,
    GR_BLACK_IS_MATED = 0x04,
    GR_STALEMATE = 0x08,
    GR_REPETITION = 0x10,
    GR_FIFTY_MOVE_RULE = 0x20,
    GR_INSUFFICIENT_MATERIAL = 0x40,
    GR_DRAW = GR_STALEMATE | GR_REPETITION | GR_FIFTY_MOVE_RULE | GR_INSUFFICIENT_MATERIAL
} game_result;

typedef struct game_data
{
    move_stack move_stack;
    game_state_stack game_state_stack;
    board board;
} game_data;

#endif // GAME_TOOLS_TYPES_H
