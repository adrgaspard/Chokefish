#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <assert.h>
#include "types.h"

static inline engine_state get_default_state();
static inline bool is_waiting_for_setup(engine_state engine_state);
static inline bool is_waiting_for_ready(engine_state engine_state);
static inline bool is_idling(engine_state engine_state);
static inline bool is_pondering(engine_state engine_state);
static inline bool is_working(engine_state engine_state);
static inline void on_sending_uciok(engine_state *engine_state);
static inline void on_sending_readyok(engine_state *engine_state);
static inline void on_starting_work(engine_state *engine_state, bool ponder);
static inline void on_switching_from_ponder_to_search(engine_state *engine_state);
static inline void on_cancelling_work(engine_state *engine_state);

static inline engine_state get_default_state()
{
    return WAITING_FOR_SETUP;
}

static inline bool is_waiting_for_setup(engine_state engine_state)
{
    return engine_state == WAITING_FOR_SETUP;
}

static inline bool is_waiting_for_ready(engine_state engine_state)
{
    return engine_state == WAITING_FOR_READY;
}

static inline bool is_idling(engine_state engine_state)
{
    return engine_state == IDLING;
}

static inline bool is_pondering(engine_state engine_state)
{
    return engine_state == PONDERING;
}

static inline bool is_working(engine_state engine_state)
{
    return (engine_state & (SEARCHING | PONDERING)) != 0;
}

static inline void on_sending_uciok(engine_state *engine_state)
{
    assert(is_waiting_for_setup(*engine_state));
    *engine_state = WAITING_FOR_READY;
}

static inline void on_sending_readyok(engine_state *engine_state)
{
    assert(!is_waiting_for_setup(*engine_state));
    if (is_waiting_for_ready(*engine_state))
    {
        *engine_state = IDLING;
    }
}

static inline void on_starting_work(engine_state *engine_state, bool ponder)
{
    assert(is_idling(*engine_state));
    *engine_state = ponder ? PONDERING : SEARCHING;
}

static inline void on_switching_from_ponder_to_search(engine_state *engine_state)
{
    assert(is_pondering(*engine_state));
    *engine_state = SEARCHING;
}

static inline void on_cancelling_work(engine_state *engine_state)
{
    assert(is_working(*engine_state));
    *engine_state = IDLING;
}

#endif // ENGINE_STATE_H
