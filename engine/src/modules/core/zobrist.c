#include "piece.h"
#include "shared_random.h"
#include "zobrist.h"

void initialize_zobrist()
{
    uint8_t file;
    position position;
    piece_type type;
    color color;
    castling castling_white, castling_black;
    for (position = 0; position < POSITIONS_COUNT; position++)
    {
        for (color = COLOR_WHITE; color <= COLOR_BLACK; color++)
        {
            for (type = PIECE_PAWN; type <= PIECE_KING; type++)
            {
                g_piece_hashes[create_piece(color, type)][position] = (zobrist_key)get_rand_u64();
            }
        }
    }
    for (castling_white = CASTLING_NONE; castling_white <= CASTLING_BOTH; castling_white++)
    {
        for (castling_black = CASTLING_NONE; castling_black <= CASTLING_BOTH; castling_black++)
        {
            g_castling_hashes[castling_white * CASTLING_STATES_COUNT + castling_black] = (zobrist_key)get_rand_u64();
        }
    }
    for (file = 0; file < FILES_COUNT; file++)
    {
        g_en_passant_file_hashes[file] = (zobrist_key)get_rand_u64();
    }
    g_en_passant_file_hashes[FILES_COUNT] = 0;
    g_opponent_turn_hash = (zobrist_key)get_rand_u64();
}

zobrist_key compute_zobrist_key(board *board)
{
    zobrist_key key;
    position position;
    piece piece;
    key = 0;
    for (position = 0; position < POSITIONS_COUNT; position++)
    {
        piece = board->position[position];
        if (!is_piece_empty(piece))
        {
            key ^= get_piece_hash(piece, position);
        }
    }
    key ^= get_en_passant_file_hash(board->current_game_state.last_en_passant_file);
    if (board->color_to_move == COLOR_BLACK)
    {
        key ^= g_opponent_turn_hash;
    }
    key ^= get_castling_hash(board->current_game_state.castling_data);
    return key;
}