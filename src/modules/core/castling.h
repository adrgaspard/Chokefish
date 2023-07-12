#ifndef CASTLING_H
#define CASTLING_H

#include "types.h"

static inline bool is_castling_valid(castling castling);

static inline bool is_castling_valid(castling castling)
{
    return castling >= CASTLING_NONE && castling <= CASTLING_BOTH;
}

#endif // CASTLING_H
