#include "../core/board.h"
#include "board_data_serializer.h"

board_data board_data_from_fen_string(char *fen_string)
{
    board_data board_data;
    assert(fen_string);
    (void)fen_string;
    // TODO
    return board_data;
}

void board_data_to_fen_string(board_data board_data, char *result)
{
    assert(result);
    (void)board_data;
    (void)result;
    // TODO
}

void load_board_from_board_data(board *board, board_data board_data)
{
    position pos;
    piece piece;
    color piece_color;
    piece_type piece_type;
    zobrist_key key;
    assert(board);
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        piece = board_data.position[pos];
        board->position[pos] = piece;
        if (!is_piece_empty(piece))
        {
            piece_color = get_color(piece);
            piece_type = get_type(piece);
            set_position_to_one(&(board->color_mask[piece_color]), pos);
            if (piece_type == PIECE_KING)
            {
                board->king_position[piece_color] = pos;
            }
            else
            {
                set_position_to_one(&(board->piece_masks[piece_color][piece_type]), pos);
                add_position_to_group(&(board->piece_groups[piece_color][piece_type]), pos);
            }
            if (piece_type != PIECE_PAWN && piece_type != PIECE_KING)
            {
                board->special_piece_count++;
            }
        }
    }
    board->color_to_move = board_data.color_to_move;
    board->all_piece_mask = board->color_mask[COLOR_WHITE] | board->color_mask[COLOR_BLACK];
    update_sliders(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.half_move_count, create_empty_piece(), 0);
    key = compute_zobrist_key(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.half_move_count, create_empty_piece(), key);
    push_on_zobrist_stack(board->position_repetition_history, key);
    push_on_game_state_stack(board->game_state_history, board->current_game_state);
}

board_data board_to_board_data(board *board)
{
    board_data board_data;
    position pos;
    assert(board);
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        board_data.position[pos] = board->position[pos];
    }
    board_data.color_to_move = board->color_to_move;
    board_data.castling_data = board->current_game_state.castling_data;
    board_data.last_en_passant_file = board->current_game_state.last_en_passant_file;
    board_data.half_move_count = board->current_game_state.half_move_count;
    return board_data;
}