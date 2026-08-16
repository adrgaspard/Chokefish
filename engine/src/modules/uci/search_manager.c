#include <assert.h>
#include "../ai/engine.h"
#include "../ai/search_result.h"
#include "../core/enhanced_time.h"
#include "../core/threading.h"
#include "../game_tools/board_utils.h"
#include "bestmove_cmd_printer.h"
#include "consts.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

// Every function in this file must be called while holding the engine mutex

static void start_search(search_token *token_ptr, board *board_to_be_copied, bool ponder, uint64_t initial_search_time);
static void stop_pondering(search_token *token_ptr, uint64_t new_search_time);
static void *search_threaded(void *arg);
static void *search_cancellation_threaded(void *arg);

void create_empty_token(search_token *token, engine_mutex *mutex_ptr, engine_state *engine_state_ptr)
{
    assert(token != NULL);
    assert(mutex_ptr != NULL);
    assert(engine_state_ptr != NULL);
    token->mutex_ptr = mutex_ptr;
    token->is_empty = false;
    token->has_ponder = false;
    token->infinite_time = false;
    token->response_requested = false;
    token->ponder_option = false;
    token->debug_option = false;
    token->search_time = 0;
    token->ponder_start_time = 0;
    token->generation = 0;
    engine_atomic_bool_init(&(token->cancellation_requested));
    engine_atomic_bool_init(&(token->currently_pondering));
    reset_search_result(&(token->result), false);
    create_board(&(token->board));
    token->engine_state_ptr = engine_state_ptr;
    reset_token(token);
}

void reset_token(search_token *token)
{
    assert(token != NULL);
    token->is_empty = true;
    token->has_ponder = false;
    token->infinite_time = false;
    token->response_requested = false;
    token->ponder_option = false;
    token->debug_option = false;
    token->search_time = 0;
    token->ponder_start_time = 0;
    engine_atomic_bool_store(&(token->cancellation_requested), false);
    engine_atomic_bool_store(&(token->currently_pondering), false);
    reset_search_result(&(token->result), false);
}

void start_search_infinite(search_token *empty_token, board *board_to_be_copied, bool ponder)
{
    start_search(empty_token, board_to_be_copied, ponder, 0);
}

void start_search_timed(search_token *empty_token, board *board_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    start_search(empty_token, board_to_be_copied, ponder, initial_search_time);
}

void stop_pondering_infinite(search_token *token)
{
    stop_pondering(token, 0);
}

void stop_pondering_timed(search_token *token, uint64_t new_search_time)
{
    assert(new_search_time != 0);
    stop_pondering(token, new_search_time);
}

void cancel_search(search_token *token, bool skip_bestmove_response)
{
    assert(token != NULL);
    assert(!token->is_empty);
    if (skip_bestmove_response)
    {
        token->response_requested = false;
    }
    engine_atomic_bool_store(&(token->cancellation_requested), true);
}

static void start_search(search_token *token_ptr, board *board_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    assert(token_ptr != NULL);
    assert(token_ptr->is_empty);
    assert(board_to_be_copied != NULL);
    token_ptr->is_empty = false;
    token_ptr->has_ponder = ponder;
    token_ptr->infinite_time = initial_search_time == 0;
    token_ptr->response_requested = true;
    token_ptr->search_time = initial_search_time;
    token_ptr->ponder_start_time = ponder ? get_current_uptime() : 0;
    token_ptr->generation++;
    engine_atomic_bool_store(&(token_ptr->cancellation_requested), false);
    engine_atomic_bool_store(&(token_ptr->currently_pondering), ponder);
    copy_board(board_to_be_copied, &(token_ptr->board));
    engine_thread_create_detached(&(token_ptr->search_thread), search_threaded, token_ptr);
    if (!ponder && initial_search_time != 0)
    {
        token_ptr->cancellation_generation = token_ptr->generation;
        token_ptr->cancellation_sleep_time_ms = initial_search_time;
        engine_thread_create_detached(&(token_ptr->cancellation_thread), search_cancellation_threaded, token_ptr);
    }
}

static void stop_pondering(search_token *token_ptr, uint64_t new_search_time)
{
    assert(token_ptr != NULL);
    assert(!token_ptr->is_empty);
    assert(token_ptr->infinite_time == (new_search_time == 0));
    assert(token_ptr->has_ponder);
    assert(engine_atomic_bool_load(&(token_ptr->currently_pondering)));
    engine_atomic_bool_store(&(token_ptr->currently_pondering), false);
    if (new_search_time != 0)
    {
        token_ptr->search_time = new_search_time;
        token_ptr->cancellation_generation = token_ptr->generation;
        token_ptr->cancellation_sleep_time_ms = new_search_time;
        engine_thread_create_detached(&(token_ptr->cancellation_thread), search_cancellation_threaded, token_ptr);
    }
}

static void *search_threaded(void *arg)
{
    search_token *token;
    bool response_requested, was_cancelled;
    token = (search_token *)arg;
    assert(token != NULL);
    search(&(token->board), &(token->result), token->mutex_ptr, &(token->cancellation_requested));
    while (!engine_atomic_bool_load(&(token->cancellation_requested)) && engine_atomic_bool_load(&(token->currently_pondering)))
    {
        engine_sleep_ms(PONDER_FINISHED_CHECK_TIME_IN_MS);
    }
    engine_mutex_lock(token->mutex_ptr);
    response_requested = token->response_requested;
    was_cancelled = engine_atomic_bool_load(&(token->cancellation_requested));
    if (!was_cancelled && is_working(*(token->engine_state_ptr)))
    {
        on_work_finished(token->engine_state_ptr);
    }
    disable_debug_printing(&(token->result), response_requested && token->debug_option);
    if (response_requested)
    {
        print_bestmove_response(&(token->result), token->ponder_option);
    }
    engine_mutex_unlock(token->mutex_ptr);
    return NULL;
}

static void *search_cancellation_threaded(void *arg)
{
    search_token *token;
    uint64_t generation, sleep_time_ms;
    token = (search_token *)arg;
    assert(token != NULL);
    // The generation and duration are captured before sleeping: a stale timer can never
    // cancel a newer search, whatever happens during its sleep
    engine_mutex_lock(token->mutex_ptr);
    generation = token->cancellation_generation;
    sleep_time_ms = token->cancellation_sleep_time_ms;
    engine_mutex_unlock(token->mutex_ptr);
    engine_sleep_ms(sleep_time_ms);
    engine_mutex_lock(token->mutex_ptr);
    if (token->generation == generation && is_working(*(token->engine_state_ptr)))
    {
        engine_atomic_bool_store(&(token->cancellation_requested), true);
    }
    engine_mutex_unlock(token->mutex_ptr);
    return NULL;
}
