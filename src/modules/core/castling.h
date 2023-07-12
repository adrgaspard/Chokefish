#ifndef CASTLING_H
#define CASTLING_H

typedef enum castling {
    CASTLING_NONE = 0b00,
    CASTLING_KING = 0b01,
    CASTLING_QUEEN = 0b10,
    CASTLING_BOTH = CASTLING_KING | CASTLING_QUEEN
} castling;

static inline bool is_castling_valid(castling castling);

static inline bool is_castling_valid(castling castling)
{
    return castling >= CASTLING_NONE && castling <= CASTLING_BOTH;
}

#endif // CASTLING_H
