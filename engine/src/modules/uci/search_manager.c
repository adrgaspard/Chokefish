#include <unistd.h>
#include "../ai/engine.h"
#include "../ai/search_result.h"
#include "../core/enhanced_time.h"
#include "../game_tools/board_utils.h"
#include "bestmove_cmd_printer.h"
#include "consts.h"
#include "debug_printer.h"
#include "engine_state.h"
#include "search_manager.h"

static void start_search(con_search_token *token_ptr, board *board_to_be_copied, bool ponder, uint64_t initial_search_time);
static void stop_pondering(con_search_token *token_ptr, uint64_t new_search_time);
static void *search_threaded(void *arg);
static void *search_cancellation_threaded(void *arg);

search_token create_empty_token(con_engine_state *engine_state_ptr, con_engine_options *engine_options_ptr, con_bool *debug_ptr)
{
    search_token token;
    assert(engine_state_ptr != NULL);
    assert(engine_options_ptr != NULL);
    assert(debug_ptr != NULL);
    reset_token(&token);
    token.engine_state_ptr = engine_state_ptr;
    token.engine_options_ptr = engine_options_ptr;
    token.debug_ptr = debug_ptr;
    return token;
}

void reset_token(search_token *token)
{
    assert(token != NULL);
    token->is_empty = true;
    token->has_ponder = false;
    token->currently_pondering = false;
    token->infinite_time = false;
    token->cancellation_requested = false;
    token->response_requested = false;
    token->ponder_start_time = 0;
    token->search_time = 0;
    reset_search_result(&(token->result), false);
}

void start_search_infinite(con_search_token *empty_token, board *board_to_be_copied, bool ponder)
{
    start_search(empty_token, board_to_be_copied, ponder, 0);
}

void start_search_timed(con_search_token *empty_token, board *board_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    start_search(empty_token, board_to_be_copied, ponder, initial_search_time);
}

void stop_pondering_infinite(con_search_token *token)
{
    stop_pondering(token, 0);
}

void stop_pondering_timed(con_search_token *token, uint64_t new_search_time)
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
    token->cancellation_requested = true;
    if (!token->currently_pondering && !token->infinite_time && !token->cancellation_requested)
    {
        pthread_cancel(token->search_cancellation_thread);
    }
}

static void start_search(con_search_token *token_ptr, board *board_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    assert(token_ptr != NULL);
    assert(token_ptr->value.is_empty);
    assert(board_to_be_copied != NULL);
    token_ptr->value.is_empty = false;
    token_ptr->value.has_ponder = ponder;
    token_ptr->value.currently_pondering = ponder;
    token_ptr->value.infinite_time = initial_search_time == 0;
    token_ptr->value.cancellation_requested = false;
    token_ptr->value.response_requested = true;
    token_ptr->value.search_time = initial_search_time;
    copy_board(board_to_be_copied, &(token_ptr->value.board));
    token_ptr->value.ponder_start_time = ponder ? get_current_uptime() : 0;
    pthread_create(&(token_ptr->value.search_thread), NULL, search_threaded, token_ptr);
    if (!ponder && initial_search_time != 0)
    {
        pthread_create(&(token_ptr->value.search_cancellation_thread), NULL, search_cancellation_threaded, token_ptr);
    }
}

static void stop_pondering(con_search_token *token_ptr, uint64_t new_search_time)
{
    assert(token_ptr != NULL);
    assert(!token_ptr->value.is_empty);
    assert(token_ptr->value.infinite_time == (new_search_time == 0));
    assert(token_ptr->value.has_ponder);
    assert(token_ptr->value.currently_pondering);
    token_ptr->value.currently_pondering = false;
    if (new_search_time != 0)
    {
        token_ptr->value.search_time = new_search_time;
        pthread_create(&(token_ptr->value.search_cancellation_thread), NULL, search_cancellation_threaded, token_ptr);
    }
}

static void *search_threaded(void *arg)
{
    con_search_token *token;
    token = (con_search_token *)arg;
    assert(token != NULL);
    search(&(token->value.board), &(token->value.result), &(token->value.cancellation_requested));
    while(!token->value.cancellation_requested && token->value.currently_pondering)
    {
        usleep(PONDER_FINISHED_CHECK_TIME_IN_MS * 1000);
    }
    rdlock_con_search_token(token);
    wrlock_con_engine_state(token->value.engine_state_ptr);
    rdlock_con_bool(token->value.debug_ptr);
    rdlock_con_engine_options(token->value.engine_options_ptr);
    disable_debug_printing(&(token->value.result), token->value.response_requested && token->value.debug_ptr->value);
    if (token->value.response_requested)
    {
        print_bestmove_response(&(token->value.result), token->value.engine_options_ptr->value.ponder);
    }
    if (!token->value.cancellation_requested)
    {
        if (is_working(token->value.engine_state_ptr->value))
        {
            on_work_finished(&(token->value.engine_state_ptr->value));
        }
    }
    unlock_con_engine_options(token->value.engine_options_ptr);
    unlock_con_bool(token->value.debug_ptr);
    unlock_con_engine_state(token->value.engine_state_ptr);
    unlock_con_search_token(token);
    return NULL;
}

static void *search_cancellation_threaded(void *arg)
{
    con_search_token *token;
    uint64_t remaining_time;
    token = (con_search_token *)arg;
    assert(token != NULL);
    rdlock_con_search_token(token);
    remaining_time = token->value.search_time;
    unlock_con_search_token(token);
    while (remaining_time > CANCELLATION_CHECK_TIME_IN_MS)
    {
        remaining_time -= CANCELLATION_CHECK_TIME_IN_MS;
        usleep(CANCELLATION_CHECK_TIME_IN_MS * 1000);
        pthread_testcancel();
    }
    if (remaining_time > 0)
    {
        usleep((uint32_t)remaining_time * 1000);
        pthread_testcancel();
    }
    pthread_testcancel();
    wrlock_con_search_token(token);
    token->value.cancellation_requested = true;
    unlock_con_search_token(token);
    return NULL;
}
