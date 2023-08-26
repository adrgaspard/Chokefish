#include <unistd.h>
#include "../ai/engine.h"
#include "../ai/search_result.h"
#include "../core/enhanced_time.h"
#include "../game_tools/game_data.h"
#include "consts.h"
#include "search_manager.h"

static void start_search(search_token *token_ptr, game_data *data_to_be_copied, bool ponder, uint64_t initial_search_time);
static void stop_pondering(search_token *token_ptr, uint64_t new_search_time);
static void *search_threaded(void *arg);
static void *search_cancellation_threaded(void *arg);

search_token create_empty_token()
{
    search_token token;
    token.is_empty = true;
    token.has_ponder = false;
    token.currently_pondering = false;
    token.infinite_time = false;
    token.cancellation_requested = false;
    token.ponder_start_time = 0;
    token.search_time = 0;
    reset_search_result(&(token.result), false);
    return token;
}

void start_search_infinite(search_token *empty_token, game_data *data_to_be_copied, bool ponder)
{
    start_search(empty_token, data_to_be_copied, ponder, 0);
}

void start_search_timed(search_token *empty_token, game_data *data_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    start_search(empty_token, data_to_be_copied, ponder, initial_search_time);
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

void cancel_search(search_token *token)
{
    assert(!token->is_empty);
    token->cancellation_requested = true;
    if (!token->currently_pondering && !token->infinite_time && !token->cancellation_requested)
    {
        pthread_cancel(token->search_cancellation_thread);
    }
}

static void start_search(search_token *token_ptr, game_data *data_to_be_copied, bool ponder, uint64_t initial_search_time)
{
    assert(token_ptr != NULL && token_ptr->is_empty && data_to_be_copied != NULL);
    token_ptr->is_empty = false;
    token_ptr->has_ponder = ponder;
    token_ptr->currently_pondering = ponder;
    token_ptr->infinite_time = initial_search_time == 0;
    token_ptr->cancellation_requested = false;
    token_ptr->search_time = initial_search_time;
    copy_game_data(&(token_ptr->game_data), data_to_be_copied);
    token_ptr->ponder_start_time = ponder ? get_current_uptime() : 0;
    pthread_create(&(token_ptr->search_thread), NULL, search_threaded, token_ptr);
    if (!ponder && initial_search_time != 0)
    {
        pthread_create(&(token_ptr->search_cancellation_thread), NULL, search_cancellation_threaded, token_ptr);
    }
}

static void stop_pondering(search_token *token_ptr, uint64_t new_search_time)
{
    assert(token_ptr != NULL && !token_ptr->is_empty && token_ptr->infinite_time == (new_search_time == 0) && token_ptr->has_ponder && token_ptr->currently_pondering);
    token_ptr->currently_pondering = false;
    if (new_search_time != 0)
    {
        token_ptr->search_time = new_search_time;
        pthread_create(&(token_ptr->search_cancellation_thread), NULL, search_cancellation_threaded, token_ptr);
    }
}

static void *search_threaded(void *arg)
{
    search_token *token;
    token = (search_token *)arg;
    assert(token != NULL);
    search(&(token->game_data), &(token->result), &(token->cancellation_requested));
    return NULL;
}

static void *search_cancellation_threaded(void *arg)
{
    search_token *token;
    uint64_t remaining_time;
    token = (search_token *)arg;
    assert(token != NULL);
    remaining_time = token->search_time;
    while (remaining_time > CANCELLATION_CHECK_TIME_IN_MS)
    {
        remaining_time -= CANCELLATION_CHECK_TIME_IN_MS;
        usleep(CANCELLATION_CHECK_TIME_IN_MS);
        pthread_testcancel();
    }
    if (remaining_time > 0)
    {
        usleep((uint32_t)remaining_time);
        pthread_testcancel();
    }
    token->cancellation_requested = true;
    return NULL;
}
