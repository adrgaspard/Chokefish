#include "../core/board.h"
#include "../core/logging.h"
#include "../core/move_generator.h"
#include "../core/move_generation_result.h"
#include "engine.h"
#include "search.h"
#include "search_result.h"

void search(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested)
{
    assert(board != NULL);
    assert(search_result != NULL);
    assert(mutex != NULL);
    assert(cancellation_requested != NULL);

    engine_mutex_lock(mutex);
    reset_search_result(search_result, true);
    engine_mutex_unlock(mutex);
    search_v0(board, search_result, mutex, cancellation_requested);
}

void reset_engine_cache()
{
    // TODO : Make a true cache deletion when engine will have a cache
}
