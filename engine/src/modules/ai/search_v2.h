#ifndef SEARCH_V2_H
#define SEARCH_V2_H

#include "../core/threading.h"
#include "types.h"

void search_v2(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested);

#endif // SEARCH_V2_H
