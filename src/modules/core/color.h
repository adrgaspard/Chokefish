#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

static inline bool is_color_valid(color color);
static inline color get_opponent(color color);

static inline bool is_color_valid(color color)
{
    return color >= COLOR_NONE && color <= COLOR_BLACK;
}

static inline color get_opponent(color color)
{
    return (color + 1) % 2;
}

#endif // PLAYER_H
