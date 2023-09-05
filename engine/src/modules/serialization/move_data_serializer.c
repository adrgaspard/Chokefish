#include <string.h>
#include "../core/move.h"
#include "consts.h"
#include "move_data.h"
#include "move_data_serializer.h"
#include "position_serializer.h"

static inline promotion_type promotion_type_from_char(char promotion_char);

static inline char promotion_type_to_char(promotion_type promotion_type);

void move_to_string(move move, char *result)
{
    move_data data;
    data = move_data_from_move(move);
    move_data_to_string(data, result);
}

void move_data_to_string(move_data move_data, char *result)
{
    char promotion_char;
    assert(result != NULL);
    position_to_string(move_data.start_pos, result);
    position_to_string(move_data.dest_pos, result + POSITION_STR_LEN - 1);
    promotion_char = promotion_type_to_char(move_data.promotion_type);
    if (promotion_char != '\0')
    {
        result[MOVE_DATA_STR_PROMOTION_FIRST_INDEX] = OPTIONAL_PROMOTION_DECLARATION_CHAR;
        result[MOVE_DATA_STR_PROMOTION_FIRST_INDEX + 1] = promotion_char;
        result[MOVE_DATA_STR_PROMOTION_FIRST_INDEX + 2] = '\0';
    }
}

move_data move_data_from_string(char *string)
{
    char first_pos_str[POSITION_STR_LEN], second_pos_str[POSITION_STR_LEN];
    position first_pos, second_pos;
    size_t string_len;
    promotion_type promotion_type;
    string_len = strlen(string);
    assert(string != NULL);
    promotion_type = PROMOTION_NONE;
    strncpy(first_pos_str, string, POSITION_STR_LEN - 1);
    first_pos_str[POSITION_STR_LEN - 1] = '\0';
    strncpy(second_pos_str, string + POSITION_STR_LEN - 1, POSITION_STR_LEN - 1);
    second_pos_str[POSITION_STR_LEN - 1] = '\0';
    first_pos = position_from_string(first_pos_str);
    second_pos = position_from_string(second_pos_str);
    if (first_pos == NO_POSITION || second_pos == NO_POSITION)
    {
        return create_empty_move_data();
    }
    if (string_len > MOVE_DATA_STR_PROMOTION_FIRST_INDEX)
    {   
        switch (string[MOVE_DATA_STR_PROMOTION_FIRST_INDEX])
        {
            case OPTIONAL_PROMOTION_DECLARATION_CHAR: 
                promotion_type = string_len > MOVE_DATA_STR_PROMOTION_FIRST_INDEX + 1
                    ? promotion_type_from_char(string[MOVE_DATA_STR_PROMOTION_FIRST_INDEX + 1])
                    : PROMOTION_NONE;
                break;
            default:
                promotion_type = promotion_type_from_char(string[MOVE_DATA_STR_PROMOTION_FIRST_INDEX]);
                break;
        }
    }
    return create_move_data(first_pos, second_pos, promotion_type);
}

move move_data_to_existing_moves(move_data move_data, move *existing_moves, uint8_t existing_moves_count)
{
    uint8_t move_index;
    move current_move;
    move_flags current_move_flags;
    piece_type promotion_piece_type;
    assert(existing_moves != NULL);
    if (move_data.start_pos != NO_POSITION && move_data.dest_pos != NO_POSITION)
    {
        for (move_index = 0; move_index < existing_moves_count; move_index++)
        {
            current_move = existing_moves[move_index];
            if (get_start_pos(current_move) == move_data.start_pos && get_dest_pos(current_move) == move_data.dest_pos)
            {
                current_move_flags = get_flags(current_move);
                if (is_promotion(current_move_flags))
                {
                    promotion_piece_type = get_promotion_piece_type(current_move_flags);
                    switch (move_data.promotion_type)
                    {
                        case PROMOTION_QUEEN:
                            if (promotion_piece_type == PIECE_QUEEN)
                            {
                                return current_move;
                            }
                            break;
                        case PROMOTION_ROOK:
                            if (promotion_piece_type == PIECE_ROOK)
                            {
                                return current_move;
                            }
                            break;
                        case PROMOTION_BISHOP:
                            if (promotion_piece_type == PIECE_BISHOP)
                            {
                                return current_move;
                            }
                            break;
                        case PROMOTION_KIGHT:
                            if (promotion_piece_type == PIECE_KNIGHT)
                            {
                                return current_move;
                            }
                            break;
                        default: break;
                    }
                }
                else
                {
                    if (move_data.promotion_type == PROMOTION_NONE)
                    {
                        return current_move;
                    }
                }
            }
        }
    }
    return create_empty_movement();
}

move_data move_data_from_move(move move)
{
    move_flags flags;
    promotion_type promotion_type;
    if (is_movement_valid(move))
    {
        promotion_type = PROMOTION_NONE;
        flags = get_flags(move);
        if(is_promotion(flags))
        {
            switch (get_promotion_piece_type(flags))
            {
                case PIECE_QUEEN:
                    promotion_type = PROMOTION_QUEEN;
                    break;
                case PIECE_ROOK:
                    promotion_type = PROMOTION_ROOK;
                    break;
                case PIECE_BISHOP:
                    promotion_type = PROMOTION_BISHOP;
                    break;
                case PIECE_KNIGHT:
                    promotion_type = PROMOTION_KIGHT;
                    break;
                default:
                    break;
            }
        }
        return create_move_data(get_start_pos(move), get_dest_pos(move), promotion_type);
    }
    return create_empty_move_data();
}

static inline promotion_type promotion_type_from_char(char promotion_char)
{
    switch (promotion_char)
    {
        case QUEEN_PROMOTION_CHAR: return PROMOTION_QUEEN;
        case ROOK_PROMOTION_CHAR: return PROMOTION_ROOK;
        case BISHOP_PROMOTION_CHAR: return PROMOTION_BISHOP;
        case KNIGHT_PROMOTION_CHAR: return PROMOTION_KIGHT;
        default: return PROMOTION_NONE;
    }
}

static inline char promotion_type_to_char(promotion_type promotion_type)
{
    switch (promotion_type)
    {
        case PROMOTION_QUEEN: return QUEEN_PROMOTION_CHAR;
        case PROMOTION_ROOK: return ROOK_PROMOTION_CHAR;
        case PROMOTION_BISHOP: return BISHOP_PROMOTION_CHAR;
        case PROMOTION_KIGHT: return KNIGHT_PROMOTION_CHAR;
        default: return '\0';
    }
}