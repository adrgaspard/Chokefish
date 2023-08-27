#ifndef CORE_CONSTS_H
#define CORE_CONSTS_H

// Partially mutable consts

#define PASSIVE_HALF_MOVE_COUNT_MAX 100 // Must be lower or equal to the zobrist_stack size declaration in types.h

// Immutable consts

#define RANKS_COUNT 8
#define FILES_COUNT 8
#define POS_A1 0
#define POS_B1 1
#define POS_C1 2
#define POS_D1 3
#define POS_F1 5
#define POS_G1 6
#define POS_H1 7
#define POS_A8 56
#define POS_B8 57
#define POS_C8 58
#define POS_D8 59
#define POS_F8 61
#define POS_G8 62
#define POS_H8 63
#define POSITIONS_COUNT (RANKS_COUNT * FILES_COUNT)
#define NO_FILE -1
#define NO_RANK -1
#define NO_POSITION -1
#define PLAYERS_COUNT 2
#define CASTLING_STATES_COUNT 4
#define PIECE_INDEXES_COUNT 16
#define PIECE_TYPES_COUNT 6
#define PIECE_GROUP_MAX_SIZE 10
#define MAX_MOVES_COUNT 218
#define WHITE_EN_PASSANT_RANK 5
#define BLACK_EN_PASSANT_RANK 2
#define KNIGHT_OFFSETS_COUNT 8
#define DIRECTIONAL_OFFSETS_COUNT 8
#define ORTHO_OR_DIAG_OFFSETS_COUNT 4
#define WHITE_KING_SIDE_CASTLE_MASK (1ULL << POS_F1 | 1ULL << POS_G1)
#define WHITE_QUEEN_SIDE_CASTLE_MASK (1ULL << POS_C1 | 1ULL << POS_D1)
#define WHITE_QUEEN_SIDE_CASTLE_BLOCK_MASK (WHITE_QUEEN_SIDE_CASTLE_MASK | 1ULL << POS_B1)
#define BLACK_KING_SIDE_CASTLE_MASK (1ULL << POS_F8 | 1ULL << POS_G8)
#define BLACK_QUEEN_SIDE_CASTLE_MASK (1ULL << POS_C8 | 1ULL << POS_D8)
#define BLACK_QUEEN_SIDE_CASTLE_BLOCK_MASK (BLACK_QUEEN_SIDE_CASTLE_MASK | 1ULL << POS_B8)

#endif // CORE_CONSTS_H