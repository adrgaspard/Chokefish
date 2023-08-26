#include <stdio.h>
#include "../core/logging.h"
#include "engine.h"
#include "search_result.h"

void search(game_data *game_data, search_result *result, bool *cancellation_requested)
{
    reset_search_result(result, true);
    (void)game_data;
    (void)cancellation_requested;
    printf(FG_YELLOW "NotImplemented: search []" COLOR_RESET "\n");
    // TODO
}

void reset_engine_cache()
{
    printf(FG_YELLOW "NotImplemented: reset_engine_cache []" COLOR_RESET "\n");
    // TODO
}