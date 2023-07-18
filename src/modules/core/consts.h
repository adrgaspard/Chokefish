#ifndef CONSTS_H
#define CONSTS_H

// Partially mutable consts

#define PASSIVE_HALF_MOVE_COUNT_MAX 100 // Must be lower or equal to the zobrist_stack size declaration in types.h

// Immutable consts

#define RANKS_COUNT 8
#define FILES_COUNT 8
#define POS_A1 0
#define POS_H1 7
#define POS_A8 56
#define POS_H8 63
#define POSITIONS_COUNT RANKS_COUNT * FILES_COUNT
#define NO_FILE 255
#define NO_POSITION -1
#define PLAYERS_COUNT 2
#define CASTLING_STATES_COUNT 4
#define PIECE_INDEXES_COUNT 16
#define PIECE_TYPES_COUNT 6
#define PIECE_GROUP_MAX_SIZE 10

#endif // CONSTS_H
