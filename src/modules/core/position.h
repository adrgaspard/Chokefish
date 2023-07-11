#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include <stdint.h>

#define RANKS_COUNT 8
#define FILES_COUNT 8
#define POSITION_COUNT RANKS_COUNT * FILES_COUNT
#define NO_POSITION 255

typedef uint8_t position;

static inline bool is_position_valid(position position)
{
    return position < POSITION_COUNT || position == NO_POSITION;
}

#endif // POSITION_H