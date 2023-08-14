#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include <assert.h>

typedef enum engine_state
{
    WAITING_FOR_SETUP,
    WAITING_FOR_READY,
} engine_state;


static inline engine_state get_default_state();
static inline bool is_waiting_for_setup(engine_state engine_state);
static inline void on_sending_uciok(engine_state *engine_state);

static inline engine_state get_default_state()
{
    return WAITING_FOR_SETUP;
}

static inline bool is_waiting_for_setup(engine_state engine_state)
{
    return engine_state == WAITING_FOR_SETUP;
}

static inline void on_sending_uciok(engine_state *engine_state)
{
    assert(is_waiting_for_setup(*engine_state));
    *engine_state = WAITING_FOR_READY;
}

#endif // ENGINE_STATE_H
