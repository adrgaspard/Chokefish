#include <stdio.h>
#include "../core/board.h"
#include "../core/logging.h"
#include "../core/move_generator.h"
#include "../core/move_generation_result.h"
#include "../core/shared_random.h"
#include "engine.h"
#include "search_result.h"

void search(board *board, search_result *search_result, bool *cancellation_requested)
{
    move_generation_result generation_result;
    move_generation_options generation_options;

    reset_search_result(search_result, true);
    reset_move_generation_result(&generation_result);
    (void)cancellation_requested;
    // TODO : Make a true search and delete the previous line
    generation_options.promotion_types_to_include = PROMOTION_ALL;
    generation_options.include_quiet_moves = true;
    generate_moves(board, &generation_result, generation_options);
    pthread_rwlock_wrlock(&(search_result->lock));
    if (generation_result.moves_count != 0)
    {
        search_result->best_move = generation_result.moves[rand() % generation_result.moves_count];
        search_result->valid = true;
        do_move(board, search_result->best_move, true);
        generate_moves(board, &generation_result, generation_options);
        if (generation_result.moves_count != 0)
        {
            search_result->ponder_move = generation_result.moves[rand() % generation_result.moves_count];
        }
    }
    search_result->finished = true;
    pthread_rwlock_unlock(&(search_result->lock));
    // printf(FG_YELLOW "EngineCall: search []" COLOR_RESET "\n");
    // fflush(stdout);
}

void reset_engine_cache()
{
    // TODO : Make a true cache deletion when engine will have a cache
    // printf(FG_YELLOW "EngineCall: reset_engine_cache []" COLOR_RESET "\n");
    // fflush(stdout);
}