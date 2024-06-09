#include "../core/board.h"
#include "../core/move.h"
#include "../core/move_generator.h"
#include "evaluation.h"
#include "search.h"

typedef struct evaluation
{
    int32_t score;
    move best_move;
    move ponder_move;
    bool cancelled;
} evaluation;

static evaluation search_v0_recursive(board *board, search_result *search_result, bool *cancellation_requested, uint16_t init_depth, uint16_t depth);

void search_v0(board *board, search_result *search_result, bool *cancellation_requested)
{
    uint16_t depth;
    evaluation evaluation;
    for (depth = 1; depth <= MAX_DEPTH; depth ++) {
        search_result->depth = depth;
        evaluation = search_v0_recursive(board, search_result, cancellation_requested, depth, depth);
        if (*cancellation_requested) {
            return;
        }
        search_result->valid = true;
        search_result->best_move = evaluation.best_move;
        search_result->ponder_move = evaluation.ponder_move;
        if (IS_END_SCORE(evaluation.score))
        {
            search_result->is_mate = true;
            search_result->mate_score = (int16_t)((evaluation.score < DRAW_SCORE) ? LOSE_SCORE - evaluation.score : WIN_SCORE - evaluation.score);
        }
        else
        {
            search_result->is_mate = false;
            search_result->centipawns_score = evaluation.score / 100.0;
        }
    }
    search_result->finished = true;
}

static evaluation search_v0_recursive(board *board, search_result *search_result, bool *cancellation_requested, uint16_t init_depth, uint16_t depth)
{
    move_generation_result generation_result;
    move_generation_options generation_options;
    uint8_t i;
    evaluation result, child_evaluation;
    
    result.best_move = create_empty_movement();
    result.ponder_move = create_empty_movement();
    result.score = LOSE_SCORE;
    result.cancelled = false;

    if (depth == 0)
    {
        search_result->nodes_explored++;
        result.score = evaluation_v0(board);
        return result;
    }

    generation_options.promotion_types_to_include = PROMOTION_QUEEN_AND_KNIGHT;
    generation_options.include_quiet_moves = true;
    generate_moves(board, &generation_result, generation_options);

    if (generation_result.moves_count == 0)
    {
        result.score = generation_result.is_currently_check ? LOSE_SCORE : DRAW_SCORE;
        return result;
    }

    for (i = 0; i < generation_result.moves_count; i++)
    {
        if (*cancellation_requested)
        {
            result.cancelled = true;
            return result;
        }
        do_move(board, generation_result.moves[i], true);
        child_evaluation = search_v0_recursive(board, search_result, cancellation_requested, init_depth, depth - 1);
        child_evaluation.score = -child_evaluation.score;
        if (!result.cancelled && child_evaluation.score > result.score)
        {
            result.score = child_evaluation.score;
            result.best_move = generation_result.moves[i];
            result.ponder_move = child_evaluation.best_move;
        }
        undo_move(board, generation_result.moves[i], true);
    }

    if (result.score < DRAW_SCORE && IS_END_SCORE(result.score))
    {
        result.score += 1;
    }

    return result;
}