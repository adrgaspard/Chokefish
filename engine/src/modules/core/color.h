#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

static inline bool is_color_valid(color color);
static inline color get_opponent(color color);

static inline bool is_color_valid(color color)
{
    return color >= COLOR_WHITE && color <= COLOR_NONE;
}

static inline color get_opponent(color color)
{
    assert(is_color_valid(color) && color != COLOR_NONE);
    return (color + 1) % 2;
}

#endif // PLAYER_H
