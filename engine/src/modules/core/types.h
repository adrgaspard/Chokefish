#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdbool.h>
#include "consts.h"
#include "dynamic_collections.h"
#include "static_collections.h"

typedef uint8_t castling_data;
typedef int8_t position;
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

// For move generation, only PROMOTION_QUEEN, PROMOTION_QUEEN_AND_KNIGHT and PROMOTION_ALL can be used.
typedef enum promotion_type
{
    PROMOTION_NONE = 0,
    PROMOTION_QUEEN = 1,
    PROMOTION_ROOK = 2,
    PROMOTION_BISHOP = 4,
    PROMOTION_KIGHT = 8,
    PROMOTION_QUEEN_AND_KNIGHT = PROMOTION_QUEEN | PROMOTION_KIGHT,
    PROMOTION_ALL = PROMOTION_QUEEN | PROMOTION_ROOK | PROMOTION_BISHOP | PROMOTION_KIGHT
} promotion_type;

DECLARE_DYNAMIC_ARRAY_TYPE(bitboard, bitboard_dynamic_array)

typedef struct vector2
{
    int32_t x;
    int32_t y;
} vector2;

typedef struct index_validation_result
{
    bool valid;
    position index;
} index_validation_result;

typedef struct magic_data
{
    bitboard value;
    uint8_t shift_quantity;
    bitboard *legal_moves;
} magic_data;

typedef struct piece_group
{
    position positions[PIECE_GROUP_MAX_SIZE];
    uint8_t map[POSITIONS_COUNT];
    uint8_t count;
} piece_group;

typedef struct game_state
{
    castling_data castling_data;
    int8_t last_en_passant_file;
    uint8_t silent_move_count;
    piece captured_piece;
    zobrist_key zobrist_key;
} game_state;

DECLARE_STATIC_STACK_TYPE(game_state, 10000, game_state_stack)
DECLARE_STATIC_STACK_TYPE(move, 10000, move_stack)
DECLARE_STATIC_STACK_TYPE(zobrist_key, 100, zobrist_stack)

typedef struct board
{
    piece position[POSITIONS_COUNT];
    piece_group piece_groups[PLAYERS_COUNT][PIECE_TYPES_COUNT - 1];
    position king_position[PLAYERS_COUNT];
    bitboard piece_masks[PLAYERS_COUNT][PIECE_TYPES_COUNT - 1];
    bitboard color_mask[PLAYERS_COUNT];
    bitboard all_pieces_mask;
    bitboard orthogonal_sliders_mask[PLAYERS_COUNT];
    bitboard diagonal_sliders_mask[PLAYERS_COUNT];
    game_state current_game_state;
    color color_to_move;
    uint8_t special_piece_count;
    uint32_t ply_count;
    game_state_stack *game_state_history;
    move_stack *move_history;
    zobrist_stack *position_repetition_history;
    bool check_state;
    bool is_check_state_cached;
} board;

typedef struct move_generation_options
{
    bool include_quiet_moves;
    promotion_type promotion_types_to_include;
} move_generation_options;

typedef struct move_generation_result
{
    move moves[MAX_MOVES_COUNT];
    uint8_t moves_count;
    bool is_currently_check;
} move_generation_result;

#endif // CORE_TYPES_H
