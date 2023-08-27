#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

extern zobrist_key g_piece_hashes[PIECE_INDEXES_COUNT][POSITIONS_COUNT];
extern zobrist_key g_castling_hashes[CASTLING_STATES_COUNT * CASTLING_STATES_COUNT];
extern zobrist_key g_en_passant_file_hashes[FILES_COUNT + 1];
extern zobrist_key g_opponent_turn_hash;

void initialize_zobrist();
zobrist_key compute_zobrist_key(board *board);
static inline zobrist_key get_piece_hash(piece piece, position position);
static inline zobrist_key get_castling_hash(castling_data data);
static inline zobrist_key get_en_passant_file_hash(int8_t en_passant_file);

static inline zobrist_key get_piece_hash(piece piece, position position)
{
    return g_piece_hashes[piece][position];
}

static inline zobrist_key get_castling_hash(castling_data data)
{
    return g_castling_hashes[data];
}

static inline zobrist_key get_en_passant_file_hash(int8_t en_passant_file)
{
    return en_passant_file == NO_FILE ? g_en_passant_file_hashes[FILES_COUNT] : g_en_passant_file_hashes[en_passant_file - 1];
}

#endif // ZOBRIST_H
