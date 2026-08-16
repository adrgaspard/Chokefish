#include "color.h"

bool is_color_valid(color color)
{
    return color >= COLOR_WHITE && color <= COLOR_NONE;
}

color get_opponent(color color)
{
    assert(is_color_valid(color) && color != COLOR_NONE);
    return (color + 1) % 2;
}
