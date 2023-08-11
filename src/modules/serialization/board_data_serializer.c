#include <ctype.h>
#include <string.h>
#include "../core/board.h"
#include "board_data_serializer.h"
#include "consts.h"

#define _FEN_DELIMITER " "
#define _FEN_LENGTH_UPPER_BOUND 100
#define _FILES_COUNT_CHAR '8'

board_data board_data_from_fen_string(char *fen_string)
{
    board_data board_data;
    char fen_copy[_FEN_LENGTH_UPPER_BOUND], *fen_ptr, current_char;
    size_t first_part_len, third_part_len;
    uint32_t char_index, move_count;
    int8_t current_file, current_rank;
    color piece_color;
    piece_type piece_type;
    piece piece;
    castling castling[PLAYERS_COUNT];
    assert(fen_string);
    board_data.silent_move_count = 0;
    board_data.last_en_passant_file = NO_FILE;
    strncpy(fen_copy, fen_string, _FEN_LENGTH_UPPER_BOUND - 1);
    fen_ptr = strtok(fen_copy, _FEN_DELIMITER);
    assert(fen_ptr);
    first_part_len = strlen(fen_ptr);
    current_file = 0;
    current_rank = RANKS_COUNT - 1;
    for (char_index = 0; char_index < first_part_len; char_index++)
    {
        current_char = fen_ptr[char_index];
        if (current_char == '/')
        {
            current_file = 0;
            current_rank--;
            continue;
        }
        if (current_char >= '0' && current_char <= _FILES_COUNT_CHAR)
        {
            current_file = (int8_t)(current_file + (current_char - '0'));
            continue;
        }
        piece_color = isupper(current_char) ? COLOR_WHITE : COLOR_BLACK;
        switch (tolower(current_char))
        {
            case KING_LOWER_CHAR:
                piece_type = PIECE_KING;
                break;
            case PAWN_LOWER_CHAR:
                piece_type = PIECE_PAWN;
                break;
            case KNIGHT_LOWER_CHAR:
                piece_type = PIECE_KNIGHT;
                break;
            case BISHOP_LOWER_CHAR:
                piece_type = PIECE_BISHOP;
                break;
            case ROOK_LOWER_CHAR:
                piece_type = PIECE_ROOK;
                break;
            case QUEEN_LOWER_CHAR:
                piece_type = PIECE_QUEEN;
                break;
            default:
                piece_type = PIECE_NONE;
                break;
        }
        piece = piece_type == PIECE_NONE ? create_empty_piece() : create_piece(piece_color, piece_type);
        board_data.position[current_rank * FILES_COUNT + current_file] = piece;
        current_file++;
    }
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    assert(fen_ptr);
    board_data.color_to_move = strcmp(fen_ptr, WHITE_TO_MOVE_STR) == 0 ? COLOR_WHITE : COLOR_BLACK;
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    assert(fen_ptr);
    third_part_len = strlen(fen_ptr);
    castling[COLOR_WHITE] = CASTLING_NONE;
    castling[COLOR_BLACK] = CASTLING_NONE;
    for (char_index = 0; char_index < third_part_len; char_index++)
    {
        castling[COLOR_WHITE] |= fen_ptr[char_index] == WHITE_CASTLE_KING_SIDE_CHAR ? CASTLING_KING : 0;
        castling[COLOR_WHITE] |= fen_ptr[char_index] == WHITE_CASTLE_KING_SIDE_CHAR ? CASTLING_QUEEN : 0;
        castling[COLOR_BLACK] |= fen_ptr[char_index] == BLACK_CASTLE_KING_SIDE_CHAR ? CASTLING_KING : 0;
        castling[COLOR_BLACK] |= fen_ptr[char_index] == BLACK_CASTLE_KING_SIDE_CHAR ? CASTLING_QUEEN : 0;
    }
    board_data.castling_data = create_castling_data(castling[COLOR_WHITE], castling[COLOR_BLACK]);
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    if (fen_ptr == NULL)
    {
        return board_data;
    }
    if (fen_ptr[0] >= FIRST_FILE_NAME && fen_ptr[0] <= LAST_FILE_NAME)
    {
        board_data.last_en_passant_file = (int8_t)(fen_ptr[0] - FIRST_FILE_NAME);
    }
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    if (fen_ptr == NULL)
    {
        return board_data;
    }
    board_data.silent_move_count = (uint8_t)atoi(fen_ptr);
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    if (fen_ptr == NULL)
    {
        return board_data;
    }
    move_count = (uint32_t)atoi(fen_ptr);
    board_data.ply_count = (move_count - 1) * 2 + (board_data.color_to_move == COLOR_WHITE ? 0 : 1);
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
    board->ply_count = board_data.ply_count;
    board->all_piece_mask = board->color_mask[COLOR_WHITE] | board->color_mask[COLOR_BLACK];
    update_sliders(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.silent_move_count, create_empty_piece(), 0);
    key = compute_zobrist_key(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.silent_move_count, create_empty_piece(), key);
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
    board_data.ply_count = board->ply_count;
    board_data.silent_move_count = board->current_game_state.silent_move_count;
    return board_data;
}