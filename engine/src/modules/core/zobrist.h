#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "castling.h"
#include "piece.h"
#include "position.h"
#include "types.h"

extern zobrist_key g_piece_hashes[PIECE_INDEXES_COUNT][POSITIONS_COUNT];
extern zobrist_key g_castling_hashes[CASTLING_STATES_COUNT * CASTLING_STATES_COUNT];
extern zobrist_key g_en_passant_file_hashes[FILES_COUNT + 1];
extern zobrist_key g_opponent_turn_hash;

void initialize_zobrist();
zobrist_key compute_zobrist_key(board *board);
zobrist_key get_piece_hash(piece piece, position position);
zobrist_key get_castling_hash(castling_data data);
zobrist_key get_en_passant_file_hash(int8_t en_passant_file);

#endif // ZOBRIST_H
