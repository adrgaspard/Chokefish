#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

void initialize_zobrist_component();

zobrist_key compute_zobrist_key(board *board);

#endif // ZOBRIST_H
