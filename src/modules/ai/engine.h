#ifndef ENGINE_H
#define ENGINE_H

#include "../game_tools/types.h"
#include "types.h"

void search(game_data *game_data, search_result *result, bool *cancellation_requested);
void reset_engine_cache();

#endif // ENGINE_H
