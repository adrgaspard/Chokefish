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
} evaluation;

typedef struct search_context
{
    search_result *result;
    engine_mutex *mutex;
    engine_atomic_bool *cancellation_requested;
    uint64_t nodes_explored;
} search_context;

static evaluation search_v0_recursive(board *board, search_context *context, uint16_t depth);

// The mutex is taken to publish each completed depth iteration
static void publish_search_result(search_context *context, uint16_t depth, evaluation *evaluation)
{
    search_result *result;
    assert(context != NULL);
    assert(evaluation != NULL);
    result = context->result;
    engine_mutex_lock(context->mutex);
    result->depth = depth;
    result->nodes_explored = context->nodes_explored;
    result->best_move = evaluation->best_move;
    result->ponder_move = evaluation->ponder_move;
    if (IS_END_SCORE(evaluation->score))
    {
        result->is_mate = true;
        result->mate_score = (int16_t)((evaluation->score < DRAW_SCORE) ? LOSE_SCORE - evaluation->score : WIN_SCORE - evaluation->score);
    }
    else
    {
        result->is_mate = false;
        result->centipawns_score = evaluation->score / 100.0;
    }
    result->valid = true;
    engine_mutex_unlock(context->mutex);
}

void search_v0(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested)
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
        evaluation = search_v0_recursive(board, &context, depth);
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

static evaluation search_v0_recursive(board *board, search_context *context, uint16_t depth)
{
    move_generation_result generation_result;
    move_generation_options generation_options;
    uint8_t i;
    evaluation result, child_evaluation;

    result.best_move = create_empty_movement();
    result.ponder_move = create_empty_movement();
    result.score = LOSE_SCORE;

    if (depth == 0)
    {
        context->nodes_explored++;
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
        if (engine_atomic_bool_load(context->cancellation_requested))
        {
            return result;
        }
        do_move(board, generation_result.moves[i], true);
        child_evaluation = search_v0_recursive(board, context, depth - 1);
        child_evaluation.score = -child_evaluation.score;
        if (child_evaluation.score > result.score)
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
