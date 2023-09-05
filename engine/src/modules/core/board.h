#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include "game_state.h"
#include "move.h"
#include "piece_group.h"
#include "position.h"
#include "precomputed_board_data.h"
#include "zobrist.h"

#define _WHITE_EN_PASSANT_CAPTURE_GAP -RANKS_COUNT
#define _BLACK_EN_PASSANT_CAPTURE_GAP RANKS_COUNT
#define _CASTLING_KING_SIDE_ROOK_START(dest_pos) dest_pos + 1
#define _CASTLING_QUEEN_SIDE_ROOK_START(dest_pos) dest_pos - 2
#define _CASTLING_KING_SIDE_ROOK_DEST(dest_pos) dest_pos - 1
#define _CASTLING_QUEEN_SIDE_ROOK_DEST(dest_pos) dest_pos + 1

static inline board create_board(game_state_stack *game_state_stack, move_stack *move_stack);
static inline bool is_check(board *board);
static inline bool compute_check_state(board *board);
static inline void update_sliders(board *board);
static inline void move_piece(board *board, color color, piece piece, piece_type piece_type, position start_pos, position dest_pos);
static inline void do_move(board *board, move move, bool is_search);
static inline void undo_move(board *board, move move, bool is_search);
static inline void do_null_move(board *board);
static inline void undo_null_move(board *board);

static inline board create_board(game_state_stack *game_state_stack, move_stack *move_stack)
{
    position position;
    game_state game_state;
    color color;
    piece_type type;
    board board;
    assert(game_state_stack != NULL && game_state_stack->count == 0);
    assert(move_stack != NULL && move_stack->count == 0);
    board.all_pieces_mask = 0ULL;
    game_state.captured_piece = create_empty_piece();
    game_state.castling_data = create_castling_data(CASTLING_NONE, CASTLING_NONE);
    game_state.silent_move_count = 0;
    game_state.last_en_passant_file = NO_FILE;
    game_state.zobrist_key = 0ULL;
    board.current_game_state = game_state;
    board.color_to_move = COLOR_NONE;
    board.special_piece_count = 0;
    board.ply_count = 0;
    board.game_state_history = game_state_stack;
    board.move_history = move_stack;
    board.check_state = false;
    board.is_check_state_cached = false;
    for (position = 0; position < POSITIONS_COUNT; position++)
    {
        board.position[position] = create_empty_piece();
    }
    for (color = COLOR_WHITE; color <= COLOR_BLACK; color++)
    {
        board.king_position[color] = NO_POSITION;
        board.color_mask[color] = 0ULL;
        board.orthogonal_sliders_mask[color] = 0ULL;
        board.diagonal_sliders_mask[color] = 0ULL;
        for (type = PIECE_PAWN; type <= PIECE_QUEEN; type++)
        {
            board.piece_groups[color][type].count = 0;
            board.piece_masks[color][type] = 0ULL;
        }
    }
    return board;
}

static inline bool is_check(board *board)
{
    assert(board != NULL);
    if (!board->is_check_state_cached)
    {
        board->check_state = compute_check_state(board);
        board->is_check_state_cached = true;
    }
    return board->check_state;
}

static inline bool compute_check_state(board *board)
{
    color current_color, opponent_color;
    position king_position;
    bitboard blockers;
    assert(board != NULL);
    current_color = board->color_to_move;
    opponent_color = get_opponent(current_color);
    king_position = board->king_position[current_color];
    blockers = board->all_pieces_mask;
    if (board->orthogonal_sliders_mask[opponent_color] != 0)
    {
        if ((get_orthogonal_moves_bitboard(king_position, blockers) & board->orthogonal_sliders_mask[opponent_color]) != 0)
        {
            return true;
        }
    }
    if (board->diagonal_sliders_mask[opponent_color] != 0)
    {
        if ((get_diagonal_moves_bitboard(king_position, blockers) & board->diagonal_sliders_mask[opponent_color]) != 0)
        {
            return true;
        }
    }
    if ((board->piece_masks[opponent_color][PIECE_KNIGHT] & g_knight_attacks[king_position]) != 0)
    {
        return true;
    }
    if ((board->piece_masks[opponent_color][PIECE_PAWN] & g_pawn_attacks[current_color][king_position]) != 0)
    {
        return true;
    }
    return false;
}

static inline void move_piece(board *board, color color, piece piece, piece_type piece_type, position start_pos, position dest_pos)
{
    assert(board != NULL);
    assert(color == COLOR_WHITE || color == COLOR_BLACK);
    assert(piece != create_empty_piece() && is_piece_valid(piece));
    assert(piece_type != PIECE_NONE && is_piece_type_valid(piece_type));
    assert(piece_type == get_type(piece));
    assert(is_position_valid(start_pos));
    assert(is_position_valid(dest_pos));
    toggle_positions(&(board->color_mask[color]), start_pos, dest_pos);
    if (piece_type == PIECE_KING)
    {
        board->king_position[color] = dest_pos;
    }
    else 
    {
        toggle_positions(&(board->piece_masks[color][piece_type]), start_pos, dest_pos);
        move_position_in_group(&(board->piece_groups[color][piece_type]), start_pos, dest_pos);
    }
    board->position[start_pos] = create_empty_piece();
    board->position[dest_pos] = piece;
}

static inline void update_sliders(board *board)
{
    assert(board != NULL);
    board->diagonal_sliders_mask[COLOR_WHITE] = board->piece_masks[COLOR_WHITE][PIECE_BISHOP] | board->piece_masks[COLOR_WHITE][PIECE_QUEEN];
    board->orthogonal_sliders_mask[COLOR_WHITE] = board->piece_masks[COLOR_WHITE][PIECE_ROOK] | board->piece_masks[COLOR_WHITE][PIECE_QUEEN];
    board->diagonal_sliders_mask[COLOR_BLACK] = board->piece_masks[COLOR_BLACK][PIECE_BISHOP] | board->piece_masks[COLOR_BLACK][PIECE_QUEEN];
    board->orthogonal_sliders_mask[COLOR_BLACK] = board->piece_masks[COLOR_BLACK][PIECE_ROOK] | board->piece_masks[COLOR_BLACK][PIECE_QUEEN];
}

static inline void do_move(board *board, move move, bool is_search)
{
    // Variables declarations
    color current_color, opponent_color;
    position start_pos, dest_pos, capture_pos, castling_rook_start_pos, castling_rook_dest_pos;
    move_flags flags;
    bool en_passant, king_side_castling;
    piece moved_piece, captured_piece, castling_rook;
    piece_type moved_piece_type, captured_piece_type, promoted_piece_type;
    castling_data old_castling_data, new_castling_data;
    int8_t old_en_passant_file, new_en_passant_file;
    uint8_t move_counter;
    zobrist_key new_key;
    game_state new_game_state;
    assert(board != NULL);
    assert(is_movement_valid(move));
    assert(board->move_history->count < board->move_history->capacity);
    assert(board->game_state_history->count < board->game_state_history->capacity);

    // Variables initializations
    current_color = board->color_to_move;
    opponent_color = get_opponent(current_color);
    start_pos = get_start_pos(move);
    dest_pos = get_dest_pos(move);
    flags = get_flags(move);
    en_passant = is_en_passant(flags);
    moved_piece = board->position[start_pos];
    moved_piece_type = get_type(moved_piece);
    captured_piece = en_passant ? create_piece(opponent_color, PIECE_PAWN) : board->position[dest_pos];
    captured_piece_type = get_type(captured_piece);
    old_castling_data = board->current_game_state.castling_data;
    new_castling_data = old_castling_data;
    old_en_passant_file = board->current_game_state.last_en_passant_file;
    new_en_passant_file = NO_FILE;
    new_key = board->current_game_state.zobrist_key;
    assert(current_color == COLOR_WHITE || COLOR_BLACK);
    assert(is_piece_valid(moved_piece));
    assert(is_castling_data_valid(old_castling_data));

    // Handle normal cases : update bitboards and piece groups
    move_piece(board, current_color, moved_piece, moved_piece_type, start_pos, dest_pos);

    // Handle capture cases
    if (is_capture(flags))
    {
        assert(captured_piece_type != PIECE_NONE);
        assert(is_piece_valid(captured_piece));
        capture_pos = dest_pos;
        if (en_passant)
        {
            capture_pos = (position)(dest_pos + (current_color == COLOR_WHITE ? _WHITE_EN_PASSANT_CAPTURE_GAP : _BLACK_EN_PASSANT_CAPTURE_GAP));
            board->position[capture_pos] = create_empty_piece();
        }
        if (captured_piece_type != PIECE_PAWN)
        {
            board->special_piece_count--;
        }
        // Remove captured piece from bitboards and piece groups
        remove_position_from_group(&(board->piece_groups[opponent_color][captured_piece_type]), capture_pos);
        toggle_position(&(board->piece_masks[opponent_color][captured_piece_type]), capture_pos);
        toggle_position(&(board->color_mask[opponent_color]), capture_pos);
        new_key ^= get_piece_hash(captured_piece, capture_pos);
    }

    // Handle king move cases
    if (moved_piece_type == PIECE_KING)
    {
        new_castling_data = get_castling_data_without_both_sides(new_castling_data, current_color);
        // Handle castling cases
        if (is_castling(flags))
        {
            castling_rook = create_piece(current_color, PIECE_ROOK);
            king_side_castling = flags == MOVE_KING_CASTLE;
            castling_rook_start_pos = (position)(king_side_castling ? _CASTLING_KING_SIDE_ROOK_START(dest_pos) : _CASTLING_QUEEN_SIDE_ROOK_START(dest_pos));
            castling_rook_dest_pos = (position)(king_side_castling ? _CASTLING_KING_SIDE_ROOK_DEST(dest_pos) : _CASTLING_QUEEN_SIDE_ROOK_DEST(dest_pos));
            // Update rook position in bitboards and piece groups
            toggle_positions(&(board->piece_masks[current_color][PIECE_ROOK]), castling_rook_start_pos, castling_rook_dest_pos);
            toggle_positions(&(board->color_mask[current_color]), castling_rook_start_pos, castling_rook_dest_pos);
            move_position_in_group(&(board->piece_groups[current_color][PIECE_ROOK]), castling_rook_start_pos, castling_rook_dest_pos);
            board->position[castling_rook_start_pos] = create_empty_piece();
            board->position[castling_rook_dest_pos] = castling_rook;
            new_key ^= get_piece_hash(castling_rook, castling_rook_start_pos);
            new_key ^= get_piece_hash(castling_rook, castling_rook_dest_pos);
        }
    }

    // Handle promotion
    if (is_promotion(flags))
    {
        board->special_piece_count++;
        promoted_piece_type = get_promotion_piece_type(flags);
        // Replace pawn by promoted piece in bitboards and piece groups
        toggle_position(&(board->piece_masks[current_color][PIECE_PAWN]), dest_pos);
        toggle_position(&(board->piece_masks[current_color][promoted_piece_type]), dest_pos);
        remove_position_from_group(&(board->piece_groups[current_color][PIECE_PAWN]), dest_pos);
        add_position_to_group(&(board->piece_groups[current_color][promoted_piece_type]), dest_pos);
        board->position[dest_pos] = create_piece(current_color, promoted_piece_type);
    }

    // Handle double pawn push
    if (flags == MOVE_DOUBLE_PAWN_PUSH)
    {
        new_en_passant_file = get_file(start_pos);
        new_key ^= get_en_passant_file_hash(new_en_passant_file);
    }

    // Update castling data
    if (new_castling_data != 0)
    {
        // Update castling data if the piece goes on a initial rook position
        if (start_pos == POS_H1 || dest_pos == POS_H1)
        {
            new_castling_data = get_castling_data_without_king_side(new_castling_data, COLOR_WHITE);
        }
        else if (start_pos == POS_A1 || dest_pos == POS_A1)
        {
            new_castling_data = get_castling_data_without_queen_side(new_castling_data, COLOR_WHITE);
        }
        if (start_pos == POS_H8 || dest_pos == POS_H8)
        {
            new_castling_data = get_castling_data_without_king_side(new_castling_data, COLOR_BLACK);
        }
        else if (start_pos == POS_A8 || dest_pos == POS_A8)
        {
            new_castling_data = get_castling_data_without_queen_side(new_castling_data, COLOR_BLACK);
        }
    }

    // Update zobrist key
    new_key ^= g_opponent_turn_hash;
    new_key ^= get_piece_hash(moved_piece, start_pos);
    new_key ^= get_piece_hash(board->position[dest_pos], dest_pos);
    new_key ^= get_en_passant_file_hash(old_en_passant_file);
    if (new_castling_data != old_castling_data)
    {
        new_key ^= get_castling_hash(old_castling_data);
        new_key ^= get_castling_hash(new_castling_data);
    }

    // Change current player
    board->color_to_move = opponent_color;
    move_counter = (uint8_t)(board->current_game_state.silent_move_count + 1U);

    // Update extra bitboards
    board->all_pieces_mask = board->color_mask[COLOR_WHITE] | board->color_mask[COLOR_BLACK];
    update_sliders(board);

    // Update board game state and stacks
    if (!is_search && (moved_piece_type == PIECE_PAWN || captured_piece_type != PIECE_NONE))
    {
        move_counter = 0;
    }
    new_game_state = create_game_state(new_castling_data, new_en_passant_file, move_counter, captured_piece, new_key);
    push_on_game_state_stack(board->game_state_history, new_game_state);
    board->current_game_state = new_game_state;
    board->is_check_state_cached = false;
    if (!is_search)
    {
        push_on_move_stack(board->move_history, move);
    }
    board->ply_count++;
}

static inline void undo_move(board *board, move move, bool is_search)
{
    // Variables declarations
    color current_color, opponent_color;
    position start_pos, dest_pos, capture_pos, castling_rook_start_pos, castling_rook_dest_pos;
    move_flags flags;
    bool promotion, king_side_castling;
    piece moved_piece, captured_piece, castling_rook, promoted_piece;
    piece_type moved_piece_type, captured_piece_type, promoted_piece_type;
    assert(board != NULL);
    assert(is_movement_valid(move));
    assert(board->move_history->count > 0);
    assert(board->game_state_history->count > 0);

    // Change current player & color variables initializations
    opponent_color = board->color_to_move;
    current_color = get_opponent(opponent_color);
    board->color_to_move = current_color;
    assert(current_color == COLOR_WHITE || COLOR_BLACK);

    // Other variables initializations
    start_pos = get_start_pos(move);
    dest_pos = get_dest_pos(move);
    flags = get_flags(move);
    promotion = is_promotion(flags);
    moved_piece = promotion ? create_piece(current_color, PIECE_PAWN) : board->position[dest_pos];
    moved_piece_type = get_type(moved_piece);
    assert(is_piece_valid(moved_piece));

    // Handle promotion
    if (promotion)
    {
        promoted_piece = board->position[dest_pos];
        promoted_piece_type = get_type(promoted_piece);
        board->special_piece_count--;
        // Replace promoted piece by pawn in bitboards and piece groups
        remove_position_from_group(&(board->piece_groups[current_color][promoted_piece_type]), dest_pos);
        add_position_to_group(&(board->piece_groups[current_color][PIECE_PAWN]), dest_pos);
        toggle_position(&(board->piece_masks[current_color][promoted_piece_type]), dest_pos);
        toggle_position(&(board->piece_masks[current_color][PIECE_PAWN]), dest_pos);
    }

    // Handle normal cases : update bitboards and piece groups
    move_piece(board, current_color, moved_piece, moved_piece_type, dest_pos, start_pos);

    // Handle capture
    if (is_capture(flags))
    {
        capture_pos = dest_pos;
        captured_piece = board->current_game_state.captured_piece;
        captured_piece_type = get_type(captured_piece);
        assert(captured_piece_type != PIECE_NONE);
        assert(is_piece_valid(captured_piece));
        if (is_en_passant(flags))
        {
            capture_pos = (position)(dest_pos + (current_color == COLOR_WHITE ? _WHITE_EN_PASSANT_CAPTURE_GAP : _BLACK_EN_PASSANT_CAPTURE_GAP));
        }
        if (captured_piece_type != PIECE_PAWN)
        {
            board->special_piece_count++;
        }
        // Add back captured piece to bitboards and piece groups
        toggle_position(&(board->piece_masks[opponent_color][captured_piece_type]), capture_pos);
        toggle_position(&(board->color_mask[opponent_color]), capture_pos);
        add_position_to_group(&(board->piece_groups[opponent_color][captured_piece_type]), capture_pos);
        board->position[capture_pos] = captured_piece;
    }

    // Handle castling moves
    if (is_castling(flags))
    {
        castling_rook = create_piece(current_color, PIECE_ROOK);
        king_side_castling = flags == MOVE_KING_CASTLE;
        castling_rook_start_pos = (position)(king_side_castling ? _CASTLING_KING_SIDE_ROOK_START(dest_pos) : _CASTLING_QUEEN_SIDE_ROOK_START(dest_pos));
        castling_rook_dest_pos = (position)(king_side_castling ? _CASTLING_KING_SIDE_ROOK_DEST(dest_pos) : _CASTLING_QUEEN_SIDE_ROOK_DEST(dest_pos));
        // Update rook position in bitboards and piece groups
        toggle_positions(&(board->piece_masks[current_color][PIECE_ROOK]), castling_rook_dest_pos, castling_rook_start_pos);
        toggle_positions(&(board->color_mask[current_color]), castling_rook_dest_pos, castling_rook_start_pos);
        move_position_in_group(&(board->piece_groups[current_color][PIECE_ROOK]), castling_rook_dest_pos, castling_rook_start_pos);
        board->position[castling_rook_start_pos] = castling_rook;
        board->position[castling_rook_dest_pos] = create_empty_piece();
    }

    // Update extra bitboards
    board->all_pieces_mask = board->color_mask[COLOR_WHITE] | board->color_mask[COLOR_BLACK];
    update_sliders(board);

    // Update board game state and stacks
    if (!is_search)
    {
        pop_from_move_stack(board->move_history);
    }
    pop_from_game_state_stack(board->game_state_history);
    board->current_game_state = peek_from_game_state_stack(board->game_state_history);
    board->is_check_state_cached = false;
    board->ply_count--;
}

// Do not call it when in check
static inline void do_null_move(board *board)
{
    zobrist_key new_key;
    game_state current_game_state, new_game_state;
    assert(board != NULL);
    assert(board->game_state_history->count < board->game_state_history->capacity);
    current_game_state = board->current_game_state;
    board->color_to_move = get_opponent(board->color_to_move);
    new_key = current_game_state.zobrist_key;
    new_key ^= g_opponent_turn_hash;
    new_key ^= get_en_passant_file_hash(current_game_state.last_en_passant_file);
    new_game_state = create_game_state(current_game_state.castling_data, NO_FILE, (uint8_t)(current_game_state.silent_move_count + 1U), create_empty_piece(), new_key);
    board->current_game_state = new_game_state;
    push_on_game_state_stack(board->game_state_history, new_game_state);
    update_sliders(board);
    board->is_check_state_cached = true;
    board->check_state = false;
    board->ply_count++;
}

static inline void undo_null_move(board *board)
{
    assert(board != NULL);
    assert(board->game_state_history->count > 0);
    board->color_to_move = get_opponent(board->color_to_move);
    pop_from_game_state_stack(board->game_state_history);
    board->current_game_state = peek_from_game_state_stack(board->game_state_history);
    update_sliders(board);
    board->is_check_state_cached = true;
    board->check_state = false;
    board->ply_count--;
}

#endif // BOARD_H
