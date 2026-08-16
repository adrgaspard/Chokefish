#ifndef UCI_TYPES_H
#define UCI_TYPES_H

#include "../ai/types.h"
#include "../core/threading.h"

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

// All fields are guarded by the engine mutex, except cancellation_requested and currently_pondering which are atomics.
// The board field is exclusively owned by the search thread once a search started.
typedef struct search_token
{
    engine_mutex *mutex_ptr;
    bool is_empty;
    bool has_ponder;
    bool infinite_time;
    bool response_requested;
    bool ponder_option;
    bool debug_option;
    uint64_t search_time;
    uint64_t ponder_start_time;
    uint64_t generation;
    uint64_t cancellation_generation;
    uint64_t cancellation_sleep_time_ms;
    engine_atomic_bool cancellation_requested;
    engine_atomic_bool currently_pondering;
    search_result result;
    board board;
    engine_state *engine_state_ptr;
    engine_thread search_thread;
    engine_thread cancellation_thread;
} search_token;

#endif // UCI_TYPES_H
