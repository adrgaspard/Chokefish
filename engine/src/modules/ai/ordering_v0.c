#include "ordering_v0.h"
#include "piece_values.h"

#define CAPTURED_PIECE_MULTIPLIER 10
#define MOVING_PIECE_MULTIPLIER 1

void sort_moves_v0(board *board, move_generation_data *data, move_generation_result *result)
{
    uint8_t i, j;
    int32_t scores[MAX_MOVES_COUNT];
    move current_move;
    int32_t current_score;
    assert(board != NULL);
    assert(data != NULL);
    assert(result != NULL);
    for (i = 0; i < result->moves_count; i++)
    {
        move_flags flags = get_flags(result->moves[i]);
        piece_type movement_piece_type = get_type(board->position[get_start_pos(result->moves[i])]);
        piece_type captured_piece_type = is_en_passant(flags) ? PIECE_PAWN : get_type(board->position[get_dest_pos(result->moves[i])]);
        scores[i] = 0;
        if (is_capture(flags))
        {
            scores[i] = CAPTURED_PIECE_MULTIPLIER * get_piece_value(captured_piece_type) - MOVING_PIECE_MULTIPLIER * get_piece_value(movement_piece_type);
        }
        if (is_promotion(flags))
        {
            scores[i] += get_piece_value(get_promotion_piece_type(flags));
        }
        if (contains_position(data->opponent_all_attacks_mask, get_dest_pos(result->moves[i])))
        {
            scores[i] -= get_piece_value(movement_piece_type);
        }
    }
    for (i = 1; i < result->moves_count; i++)
    {
        current_move = result->moves[i];
        current_score = scores[i];
        j = i;
        while (j > 0 && scores[j - 1] < current_score)
        {
            result->moves[j] = result->moves[j - 1];
            scores[j] = scores[j - 1];
            j--;
        }
        result->moves[j] = current_move;
        scores[j] = current_score;
    }
}
