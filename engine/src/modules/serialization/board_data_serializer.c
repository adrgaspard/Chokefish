#include <ctype.h>
#include <string.h>
#include "../core/board.h"
#include "board_data_serializer.h"
#include "consts.h"
#include "position_serializer.h"

#define _FEN_DELIMITER " "
#define _SILENT_MOVE_COUNT_LENGTH_UPPER_BOUND 10
#define _FILES_COUNT_CHAR '8'

static bool en_passant_can_be_captured(board *board, int8_t en_passant_file, int8_t en_passant_rank);
static bool is_valid_en_passant_capture(board *board, vector2 from_pos_vector, position en_passant_capture_pos);

board_data board_data_from_fen_string(char *fen_string)
{
    board_data board_data;
    char fen_copy[FEN_LENGTH_UPPER_BOUND + 1], *fen_ptr, current_char;
    size_t first_part_len, third_part_len;
    uint32_t char_index, move_count;
    int8_t current_file, current_rank;
    color piece_color;
    piece_type piece_type;
    piece piece;
    castling castling[PLAYERS_COUNT];
    position pos;
    assert(fen_string != NULL);
    board_data.silent_move_count = 0;
    board_data.last_en_passant_file = NO_FILE;
    board_data.ply_count = 0;
    strncpy(fen_copy, fen_string, FEN_LENGTH_UPPER_BOUND - 1);
    fen_ptr = strtok(fen_copy, _FEN_DELIMITER);
    assert(fen_ptr != NULL);
    first_part_len = strlen(fen_ptr);
    current_file = 0;
    current_rank = RANKS_COUNT - 1;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        board_data.position[pos] = create_empty_piece();
    }
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
    assert(fen_ptr != NULL);
    board_data.color_to_move = strcmp(fen_ptr, WHITE_TO_MOVE_STR) == 0 ? COLOR_WHITE : COLOR_BLACK;
    fen_ptr = strtok(NULL, _FEN_DELIMITER);
    assert(fen_ptr != NULL);
    third_part_len = strlen(fen_ptr);
    castling[COLOR_WHITE] = CASTLING_NONE;
    castling[COLOR_BLACK] = CASTLING_NONE;
    for (char_index = 0; char_index < third_part_len; char_index++)
    {
        castling[COLOR_WHITE] |= fen_ptr[char_index] == WHITE_CASTLE_KING_SIDE_CHAR ? CASTLING_KING : CASTLING_NONE;
        castling[COLOR_WHITE] |= fen_ptr[char_index] == WHITE_CASTLE_QUEEN_SIDE_CHAR ? CASTLING_QUEEN : CASTLING_NONE;
        castling[COLOR_BLACK] |= fen_ptr[char_index] == BLACK_CASTLE_KING_SIDE_CHAR ? CASTLING_KING : CASTLING_NONE;
        castling[COLOR_BLACK] |= fen_ptr[char_index] == BLACK_CASTLE_QUEEN_SIDE_CHAR ? CASTLING_QUEEN : CASTLING_NONE;
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

void board_to_fen_string(board *board, char *result)
{
    uint8_t write_index, empty_files_count;
    int8_t rank, file, en_passant_rank;
    position pos;
    piece piece;
    piece_type piece_type;
    color piece_color;
    char piece_char, en_passant_str[POSITION_STR_LEN];
    bool no_castling;
    assert(board != NULL);
    assert(result != NULL);
    write_index = 0;
    
    // Pieces on the board
    for (rank = RANKS_COUNT - 1; rank >= 0; rank--)
    {
        empty_files_count = 0;
        for (file = 0; file < FILES_COUNT; file++)
        {
            pos = compute_index_if_valid(file, rank).index;
            piece = board->position[pos];
            if (!is_piece_empty(piece))
            {
                piece_type = get_type(piece);
                piece_color = get_color(piece);
                if (empty_files_count != 0)
                {
                    result[write_index++] = (char)('0' + empty_files_count);
                    empty_files_count = 0;
                }
                switch (piece_type)
                {
                    case PIECE_KING:
                        piece_char = KING_LOWER_CHAR;
                        break;
                    case PIECE_PAWN:
                        piece_char = PAWN_LOWER_CHAR;
                        break;
                    case PIECE_KNIGHT:
                        piece_char = KNIGHT_LOWER_CHAR;
                        break;
                    case PIECE_BISHOP:
                        piece_char = BISHOP_LOWER_CHAR;
                        break;
                    case PIECE_ROOK:
                        piece_char = ROOK_LOWER_CHAR;
                        break;
                    case PIECE_QUEEN:
                        piece_char = QUEEN_LOWER_CHAR;
                        break;
                    default:
                        piece_char = 'X';
                        break;
                }
                result[write_index++] = (char)(piece_color == COLOR_WHITE ? toupper(piece_char) : piece_char);
            }
            else
            {
                empty_files_count++;
            }
        }
        if (empty_files_count != 0)
        {
            result[write_index++] = (char)('0' + empty_files_count);
        }
        if (rank != 0)
        {
            result[write_index++] = '/';
        }
    }

    // Color to move
    result[write_index++] = ' ';
    result[write_index++] = board->color_to_move == COLOR_WHITE ? WHITE_TO_MOVE_CHAR : BLACK_TO_MOVE_CHAR;

    // Castling
    result[write_index++] = ' ';
    no_castling = true;
    if ((get_white_castling_right(board->current_game_state.castling_data) & CASTLING_KING) != 0)
    {
        result[write_index++] = WHITE_CASTLE_KING_SIDE_CHAR;
        no_castling = false;
    }
    if ((get_white_castling_right(board->current_game_state.castling_data) & CASTLING_QUEEN) != 0)
    {
        result[write_index++] = WHITE_CASTLE_QUEEN_SIDE_CHAR;
        no_castling = false;
    }
    if ((get_black_castling_right(board->current_game_state.castling_data) & CASTLING_KING) != 0)
    {
        result[write_index++] = BLACK_CASTLE_KING_SIDE_CHAR;
        no_castling = false;
    }
    if ((get_black_castling_right(board->current_game_state.castling_data) & CASTLING_QUEEN) != 0)
    {
        result[write_index++] = BLACK_CASTLE_QUEEN_SIDE_CHAR;
        no_castling = false;
    }
    if (no_castling)
    {
        result[write_index++] = SKIP_CHAR;
    }

    // En passant
    result[write_index++] = ' ';
    en_passant_rank = board->color_to_move == COLOR_WHITE ? WHITE_EN_PASSANT_RANK : BLACK_EN_PASSANT_RANK;
    if (board->current_game_state.last_en_passant_file == NO_FILE || !en_passant_can_be_captured(board, board->current_game_state.last_en_passant_file, en_passant_rank))
    {
        result[write_index++] = SKIP_CHAR;
    }
    else
    {
        position_to_string(compute_index_if_valid(board->current_game_state.last_en_passant_file, en_passant_rank).index, en_passant_str);
        result[write_index++] = en_passant_str[0];
        result[write_index++] = en_passant_str[1];
    }

    // Silent move counter
    result[write_index++] = ' ';
    write_index = (uint8_t)(write_index + sprintf(result + write_index, "%u", board->current_game_state.silent_move_count));

    // Full move counter
    result[write_index++] = ' ';
    write_index = (uint8_t)(write_index + sprintf(result + write_index, "%u", board->ply_count / 2U + 1U));
    result[write_index] = '\0';
}

void load_board_from_board_data(board *board, board_data board_data)
{
    position pos;
    piece piece;
    color piece_color;
    piece_type piece_type;
    zobrist_key key;
    assert(board != NULL);
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
    board->all_pieces_mask = board->color_mask[COLOR_WHITE] | board->color_mask[COLOR_BLACK];
    update_sliders(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.silent_move_count, create_empty_piece(), 0);
    key = compute_zobrist_key(board);
    board->current_game_state = create_game_state(board_data.castling_data, board_data.last_en_passant_file, board_data.silent_move_count, create_empty_piece(), key);
    push_on_game_state_stack(&(board->game_state_history), board->current_game_state);
}

static bool en_passant_can_be_captured(board *board, int8_t en_passant_file, int8_t en_passant_rank)
{
    vector2 capture_from_left, capture_from_right;
    index_validation_result en_passant_capture_pos_result;
    en_passant_capture_pos_result = compute_index_if_valid(en_passant_file, en_passant_rank); 
    if (en_passant_capture_pos_result.valid)
    {
        capture_from_left.x = en_passant_file - 1;
        capture_from_left.y = en_passant_rank + (board->color_to_move == COLOR_WHITE ? -1 : 1);
        capture_from_right.x = en_passant_file + 1;
        capture_from_right.y = en_passant_rank + (board->color_to_move == COLOR_WHITE ? -1 : 1);
        return is_valid_en_passant_capture(board, capture_from_left, en_passant_capture_pos_result.index)
            || is_valid_en_passant_capture(board, capture_from_right, en_passant_capture_pos_result.index);
    }
    return false;
}

static bool is_valid_en_passant_capture(board *board, vector2 from_pos_vector, position en_passant_capture_pos)
{
    move move;
    bool is_legal;
    index_validation_result from_pos_result;
    from_pos_result = compute_index_if_valid(from_pos_vector.x, from_pos_vector.y);
    if (from_pos_result.valid && board->position[from_pos_result.index] == create_piece(board->color_to_move, PIECE_PAWN))
    {
        move = create_movement(from_pos_result.index, en_passant_capture_pos, MOVE_EN_PASSANT_CAPTURE);
        do_move(board, move, false);
        do_null_move(board);
        is_legal = !compute_check_state(board);
        undo_null_move(board);
        undo_move(board, move, false);
        return is_legal;
    }
    return false;
}
