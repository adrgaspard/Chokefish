#include "../core/board.h"
#include "../core/logging.h"
#include "../core/move_generator.h"
#include "../core/move_generation_result.h"
#include "engine.h"
#include "search.h"
#include "search_result.h"

void search(board *board, search_result *search_result, bool *cancellation_requested)
{
    assert(board != NULL);
    assert(search_result != NULL);
    assert(cancellation_requested != NULL);

    reset_search_result(search_result, true);
    search_v0(board, search_result, cancellation_requested);
}

void reset_engine_cache()
{
    // TODO : Make a true cache deletion when engine will have a cache
}