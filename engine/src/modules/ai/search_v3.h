#ifndef SEARCH_V3_H
#define SEARCH_V3_H

#include "../core/threading.h"
#include "types.h"

void search_v3(board *board, search_result *search_result, engine_mutex *mutex, engine_atomic_bool *cancellation_requested);

#endif // SEARCH_V3_H
