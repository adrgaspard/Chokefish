#include <assert.h>
#include "engine_state.h"

engine_state get_default_state()
{
    return WAITING_FOR_SETUP;
}

bool is_waiting_for_setup(engine_state engine_state)
{
    return engine_state == WAITING_FOR_SETUP;
}

bool is_waiting_for_ready(engine_state engine_state)
{
    return engine_state == WAITING_FOR_READY;
}

bool is_idling(engine_state engine_state)
{
    return engine_state == IDLING;
}

bool is_pondering(engine_state engine_state)
{
    return engine_state == PONDERING;
}

bool is_working(engine_state engine_state)
{
    return (engine_state & (SEARCHING | PONDERING)) != 0;
}

void on_sending_uciok(engine_state *engine_state)
{
    assert(engine_state != NULL);
    assert(is_waiting_for_setup(*engine_state));
    *engine_state = WAITING_FOR_READY;
}

void on_sending_readyok(engine_state *engine_state)
{
    assert(engine_state != NULL);
    assert(!is_waiting_for_setup(*engine_state));
    if (is_waiting_for_ready(*engine_state))
    {
        *engine_state = IDLING;
    }
}

void on_starting_work(engine_state *engine_state, bool ponder)
{
    assert(engine_state != NULL);
    assert(is_idling(*engine_state));
    *engine_state = ponder ? PONDERING : SEARCHING;
}

void on_switching_from_ponder_to_search(engine_state *engine_state)
{
    assert(engine_state != NULL);
    assert(is_pondering(*engine_state));
    *engine_state = SEARCHING;
}

void on_cancelling_work(engine_state *engine_state)
{
    assert(engine_state != NULL);
    assert(is_working(*engine_state));
    *engine_state = IDLING;
}

void on_work_finished(engine_state *engine_state)
{
    assert(engine_state != NULL);
    assert(is_working(*engine_state));
    *engine_state = IDLING;
}