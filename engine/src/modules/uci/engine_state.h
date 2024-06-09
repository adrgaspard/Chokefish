#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include "types.h"

engine_state get_default_state();
bool is_waiting_for_setup(engine_state engine_state);
bool is_waiting_for_ready(engine_state engine_state);
bool is_idling(engine_state engine_state);
bool is_pondering(engine_state engine_state);
bool is_working(engine_state engine_state);
void on_sending_uciok(engine_state *engine_state);
void on_sending_readyok(engine_state *engine_state);
void on_starting_work(engine_state *engine_state, bool ponder);
void on_switching_from_ponder_to_search(engine_state *engine_state);
void on_cancelling_work(engine_state *engine_state);
void on_work_finished(engine_state *engine_state);


#endif // ENGINE_STATE_H
