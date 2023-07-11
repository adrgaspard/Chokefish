#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>
#include "board.h"

typedef uint64_t zobrist_key;

void initialize_zobrist();

zobrist_key compute_zobrist_key(board *board);

#endif // ZOBRIST_H
