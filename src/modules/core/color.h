#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

static inline bool is_color_valid(color color)
{
    return color >= COLOR_NONE && color <= COLOR_BLACK;
}

#endif // PLAYER_H
