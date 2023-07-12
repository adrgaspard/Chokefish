#ifndef BOARD_TYPES_H
#define BOARD_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "consts.h"

typedef uint8_t position;
typedef uint8_t piece;
typedef uint16_t move;
typedef uint64_t zobrist_key;
typedef uint64_t bitboard;

typedef enum piece_type
{
    PIECE_PAWN = 0,
    PIECE_KNIGHT = 1,
    PIECE_BISHOP = 2,
    PIECE_ROOK = 3,
    PIECE_QUEEN = 4,
    PIECE_KING = 5,
    PIECE_NONE = 6
} piece_type;

typedef enum color
{
    COLOR_WHITE = 0,
    COLOR_BLACK = 1,
    COLOR_NONE = 2
} color;

typedef enum move_flags
{
    MOVE_QUIET = 0x0,
    MOVE_DOUBLE_PAWN_PUSH = 0x1,
    MOVE_KING_CASTLE = 0x2,
    MOVE_QUEEN_CASTLE = 0x3,
    MOVE_CAPTURE = 0x4,
    MOVE_EN_PASSANT_CAPTURE = 0x5,
    MOVE_KNIGHT_PROMOTION = 0x8,
    MOVE_BISHOP_PROMOTION = 0x9,
    MOVE_ROOK_PROMOTION = 0xA,
    MOVE_QUEEN_PROMOTION = 0xB,
    MOVE_KNIGHT_PROMOTION_CAPTURE = 0xC,
    MOVE_BISHOP_PROMOTION_CAPTURE = 0xD,
    MOVE_ROOK_PROMOTION_CAPTURE = 0xE,
    MOVE_QUEEN_PROMOTION_CAPTURE = 0xF
} move_flags;

typedef enum castling {
    CASTLING_NONE = 0x0,
    CASTLING_KING = 0x1,
    CASTLING_QUEEN = 0x2,
    CASTLING_BOTH = CASTLING_KING | CASTLING_QUEEN
} castling;

typedef struct piece_group
{
    position positions[PIECE_GROUP_MAX_SIZE];
    uint8_t map[POSITIONS_COUNT];
    uint8_t count;
} piece_group;

typedef struct board
{
    zobrist_key key;
    // TODO
} board;

#endif // BOARD_TYPES_H
