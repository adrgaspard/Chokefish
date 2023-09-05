#ifndef SEARCH_MANAGER_H
#define SEARCH_MANAGER_H

#include "types.h"

search_token create_empty_token(engine_state *engine_state_ptr, engine_options *engine_options_ptr, bool *debug_ptr);
void reset_token(search_token *token);
void start_search_infinite(search_token *empty_token, board *board_to_be_copied, bool ponder);
void start_search_timed(search_token *empty_token, board *board_to_be_copied, bool ponder, uint64_t initial_search_time);
void stop_pondering_infinite(search_token *token);
void stop_pondering_timed(search_token *token, uint64_t new_search_time);
void cancel_search(search_token *token, bool skip_bestmove_response);

#endif // SEARCH_MANAGER_H
