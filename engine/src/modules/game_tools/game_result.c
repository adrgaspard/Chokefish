#include "../core/move_generation_result.h"
#include "../core/move_generator.h"
#include "game_result.h"

static move_generation_options s_all_moves = { .include_quiet_moves = true, .promotion_types_to_include = PROMOTION_ALL };

static bool is_insufficient_material(board *board);

game_result get_game_result(board *board)
{
    uint32_t repetition_count, i;
    zobrist_key current_key;
    move_generation_result generation_result;
    assert(board != NULL);
    assert(board->game_state_history.count > 0);
    reset_move_generation_result(&generation_result);
    generate_moves(board, &generation_result, s_all_moves);
    if (generation_result.moves_count == 0)
    {
        if (generation_result.is_currently_check)
        {
            return board->color_to_move == COLOR_WHITE ? GR_WHITE_IS_MATED : GR_BLACK_IS_MATED;
        }
        return GR_STALEMATE;
    }
    if (board->current_game_state.silent_move_count >= PASSIVE_HALF_MOVE_COUNT_MAX)
    {
        return GR_FIFTY_MOVE_RULE;
    }
    current_key = peek_from_game_state_stack(&(board->game_state_history)).zobrist_key;
    repetition_count = 1;
        for (i = board->game_state_history.count - board->current_game_state.silent_move_count; i < board->game_state_history.count - 1; i++)
        {
        if (board->game_state_history.items[i].zobrist_key == current_key)
        {
            repetition_count++;
            if (repetition_count >= REPETITION_RULE_COUNT)
            {
                return GR_REPETITION;
            }
        }
    }
    return is_insufficient_material(board) ? GR_INSUFFICIENT_MATERIAL : GR_PLAYING;
}

static bool is_insufficient_material(board *board)
{
    uint8_t white_bishops_count, white_knights_count, black_bishops_count, black_knights_count, white_minors_count, black_minors_count;
    assert(board != NULL);
    if (board->piece_masks[COLOR_WHITE][PIECE_PAWN] != 0 || board->piece_masks[COLOR_BLACK][PIECE_PAWN] != 0 || board->orthogonal_sliders_mask[COLOR_WHITE] != 0 || board->orthogonal_sliders_mask[COLOR_BLACK])
    {
        return false;
    }
    white_bishops_count = board->piece_groups[COLOR_WHITE][PIECE_BISHOP].count;
    white_knights_count = board->piece_groups[COLOR_WHITE][PIECE_KNIGHT].count;
    white_minors_count = white_bishops_count + white_knights_count;
    black_bishops_count = board->piece_groups[COLOR_BLACK][PIECE_BISHOP].count;
    black_knights_count = board->piece_groups[COLOR_BLACK][PIECE_KNIGHT].count;
    black_minors_count = black_bishops_count + black_knights_count;
    return white_minors_count <= 1 && black_minors_count <= 1;
}
