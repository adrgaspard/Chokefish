#ifndef ENGINE_H
#define ENGINE_H

#include "../core/types.h"
#include "types.h"

void search(board *board, search_result *search_result, bool *cancellation_requested);
void reset_engine_cache();

#endif // ENGINE_H
