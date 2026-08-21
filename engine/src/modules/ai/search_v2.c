#include "../core/board.h"
#include "../core/move.h"
#include "../core/move_generator.h"
#include "ordering_v1.h"
#include "search_utils.h"
#include "evaluation_v1.h"
#include "search_v2.h"

static evaluation search_v2_recursive(board *board, search_context *context, uint16_t depth, int32_t alpha, int32_t beta, move priority_move);
static int32_t search_v2_quiescence(board *board, search_context *context, int32_t alpha, int32_t beta);

void search_v2(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested)
{
    uint16_t depth;
    evaluation evaluation;
    move previous_best_move;
    search_context context;
    assert(board != NULL);
    assert(search_result != NULL);
    assert(mutex != NULL);
    assert(cancellation_requested != NULL);
    context.result = search_result;
    context.mutex = mutex;
    context.cancellation_requested = cancellation_requested;
    context.nodes_explored = 0;
    previous_best_move = create_empty_movement();
    for (depth = 1; depth <= MAX_DEPTH; depth++)
    {
        evaluation = search_v2_recursive(board, &context, depth, LOSE_SCORE, WIN_SCORE, previous_best_move);
        if (engine_atomic_bool_load(cancellation_requested))
        {
            return;
        }
        publish_search_result(&context, depth, &evaluation);
        previous_best_move = evaluation.best_move;
    }
    engine_mutex_lock(mutex);
    search_result->finished = true;
    engine_mutex_unlock(mutex);
}

static evaluation search_v2_recursive(board *board, search_context *context, uint16_t depth, int32_t alpha, int32_t beta, move priority_move)
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
        result.score = search_v2_quiescence(board, context, alpha, beta);
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

    sort_moves_v1(board, &generation_data, &generation_result, priority_move);

    for (i = 0; i < generation_result.moves_count; i++)
    {
        if (engine_atomic_bool_load(context->cancellation_requested))
        {
            return result;
        }
        do_move(board, generation_result.moves[i], true);
        child_evaluation = search_v2_recursive(board, context, depth - 1, -beta, -alpha, create_empty_movement());
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

static int32_t search_v2_quiescence(board *board, search_context *context, int32_t alpha, int32_t beta)
{
    move_generation_result generation_result;
    move_generation_data generation_data;
    move_generation_options generation_options;
    uint8_t i;
    int32_t stand_pat, score;
    context->nodes_explored++;
    if ((context->nodes_explored & 1023U) == 0)
    {
        engine_atomic_uint64_store(&(context->result->nodes_explored), context->nodes_explored);
    }
    stand_pat = evaluation_v1(board);
    if (stand_pat >= beta)
    {
        return beta;
    }
    if (stand_pat > alpha)
    {
        alpha = stand_pat;
    }
    generation_options.promotion_types_to_include = PROMOTION_QUEEN_AND_KNIGHT;
    generation_options.include_quiet_moves = false;
    generate_moves_with_data(board, &generation_result, generation_options, &generation_data);
    if (generation_result.is_currently_check)
    {
        // When in check, quiet evasions must also be considered
        generation_options.include_quiet_moves = true;
        generate_moves_with_data(board, &generation_result, generation_options, &generation_data);
    }
    if (generation_result.moves_count == 0)
    {
        return generation_result.is_currently_check ? LOSE_SCORE : DRAW_SCORE;
    }
    sort_moves_v1(board, &generation_data, &generation_result, create_empty_movement());
    for (i = 0; i < generation_result.moves_count; i++)
    {
        if (engine_atomic_bool_load(context->cancellation_requested))
        {
            return alpha;
        }
        do_move(board, generation_result.moves[i], true);
        score = -search_v2_quiescence(board, context, -beta, -alpha);
        undo_move(board, generation_result.moves[i], true);
        if (score >= beta)
        {
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}
