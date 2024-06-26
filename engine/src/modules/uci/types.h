#ifndef UCI_TYPES_H
#define UCI_TYPES_H

#include <pthread.h>
#include "../ai/types.h"

typedef enum engine_state
{
    WAITING_FOR_SETUP = 0x01,
    WAITING_FOR_READY = 0x02,
    IDLING = 0x04,
    SEARCHING = 0x08,
    PONDERING = 0x10
} engine_state;

typedef struct engine_options
{
    bool ponder;
    bool own_book;
    int64_t threads_count;
    int64_t threads_count_min;
    int64_t threads_count_max;
} engine_options;

typedef struct search_token
{
    pthread_mutex_t *global_mutex_ptr;
    bool is_empty;
    bool has_ponder;
    bool currently_pondering;
    bool infinite_time;
    bool cancellation_requested;
    bool response_requested;
    uint64_t search_time;
    uint64_t ponder_start_time;
    pthread_t search_thread;
    pthread_attr_t search_thread_attribute;
    pthread_t search_cancellation_thread;
    pthread_attr_t search_cancellation_thread_attribute;
    search_result result;
    board board;
    engine_state *engine_state_ptr;
    engine_options *engine_options_ptr;
    bool *debug_ptr;
} search_token;

#endif // UCI_TYPES_H
