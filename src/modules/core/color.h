#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define PLAYERS_COUNT 2

typedef enum color
{
    COLOR_WHITE = 0,
    COLOR_BLACK = 1,
    COLOR_NONE = 2
} color;

static inline bool is_color_valid(color color)
{
    return color >= COLOR_NONE && color <= COLOR_BLACK;
}

#endif // PLAYER_H
