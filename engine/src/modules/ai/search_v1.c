#include "../core/board.h"
#include "../core/move.h"
#include "../core/move_generator.h"
#include "ordering_v0.h"
#include "search_utils.h"
#include "evaluation.h"
#include "search.h"

static evaluation search_v1_recursive(board *board, search_context *context, uint16_t depth, int32_t alpha, int32_t beta);

void search_v1(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested)
{
    uint16_t depth;
    evaluation evaluation;
    search_context context;
    assert(board != NULL);
    assert(search_result != NULL);
    assert(mutex != NULL);
    assert(cancellation_requested != NULL);
    context.result = search_result;
    context.mutex = mutex;
    context.cancellation_requested = cancellation_requested;
    context.nodes_explored = 0;
    for (depth = 1; depth <= MAX_DEPTH; depth++)
    {
        evaluation = search_v1_recursive(board, &context, depth, LOSE_SCORE, WIN_SCORE);
        if (engine_atomic_bool_load(cancellation_requested))
        {
            return;
        }
        publish_search_result(&context, depth, &evaluation);
    }
    engine_mutex_lock(mutex);
    search_result->finished = true;
    engine_mutex_unlock(mutex);
}

static evaluation search_v1_recursive(board *board, search_context *context, uint16_t depth, int32_t alpha, int32_t beta)
{
    move_generation_result generation_result;
    move_generation_data generation_data;
    move_generation_options generation_options;
    uint8_t i;
    evaluation result, child_evaluation;

    result.best_move = create_empty_movement();
    result.ponder_move = create_empty_movement();
    result.score = LOSE_SCORE;

    if (depth == 0)
    {
        context->nodes_explored++;
        if ((context->nodes_explored & 1023U) == 0)
        {
            engine_atomic_uint64_store(&(context->result->nodes_explored), context->nodes_explored);
        }
        result.score = evaluation_v0(board);
        return result;
    }

    generation_options.promotion_types_to_include = PROMOTION_QUEEN_AND_KNIGHT;
    generation_options.include_quiet_moves = true;
    generate_moves_with_data(board, &generation_result, generation_options, &generation_data);

    if (generation_result.moves_count == 0)
    {
        result.score = generation_result.is_currently_check ? LOSE_SCORE : DRAW_SCORE;
        return result;
    }

    sort_moves_v0(board, &generation_data, &generation_result);

    for (i = 0; i < generation_result.moves_count; i++)
    {
        if (engine_atomic_bool_load(context->cancellation_requested))
        {
            return result;
        }
        do_move(board, generation_result.moves[i], true);
        child_evaluation = search_v1_recursive(board, context, depth - 1, -beta, -alpha);
        child_evaluation.score = -child_evaluation.score;
        undo_move(board, generation_result.moves[i], true);
        if (i == 0 || child_evaluation.score > result.score)
        {
            result.score = child_evaluation.score;
            result.best_move = generation_result.moves[i];
            result.ponder_move = child_evaluation.best_move;
        }
        if (result.score >= beta)
        {
            return result;
        }
        if (result.score > alpha)
        {
            alpha = result.score;
        }
    }

    if (result.score < DRAW_SCORE && IS_END_SCORE(result.score))
    {
        result.score += 1;
    }

    return result;
}
