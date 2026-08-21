#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

void search_v0(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested);
void search_v1(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested);

#endif // SEARCH_H
