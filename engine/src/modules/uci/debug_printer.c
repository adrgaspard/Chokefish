#include <stdbool.h>
#include <stdio.h>
#include "../ai/types.h"
#include "../core/enhanced_time.h"
#include "../core/logging.h"
#include "consts.h"
#include "debug_printer.h"

#define LOOP_TIME_IN_MS 100

// All fields below are guarded by the engine mutex

static engine_mutex *s_mutex;
static search_result *s_current_result;
static bool s_enabled;
static engine_thread s_thread;

static void *debug_print_loop(void *arg);
static void print_search_result_info(search_result *result);

void initialize_debug_printer(engine_mutex *mutex)
{
    assert(mutex != NULL);
    s_mutex = mutex;
    s_current_result = NULL;
    s_enabled = false;
    engine_thread_create_detached(&s_thread, debug_print_loop, NULL);
}

void enable_debug_printing(search_result *result)
{
    assert(result != NULL);
    s_current_result = result;
    s_enabled = true;
}

void disable_debug_printing(search_result *result, bool print_one_last_time)
{
    assert(result != NULL);
    if (s_enabled)
    {
        if (print_one_last_time)
        {
            print_search_result_info(result);
        }
        s_enabled = false;
        s_current_result = NULL;
    }
}

static void *debug_print_loop(void *arg)
{
    (void)arg;
    while (true)
    {
        engine_mutex_lock(s_mutex);
        if (s_enabled && s_current_result != NULL && s_current_result->valid)
        {
            print_search_result_info(s_current_result);
        }
        engine_mutex_unlock(s_mutex);
        engine_sleep_ms(LOOP_TIME_IN_MS);
    }
    return NULL;
}

// Must be called while holding the engine mutex
static void print_search_result_info(search_result *result)
{
    uint64_t time;
    assert(result != NULL);
    time = get_current_uptime() - result->start_time;
    printf(EG_CMD_INFO UCI_DELIMITER EG_CMD_INFO_OPT_DEPTH  " " U16 " " EG_CMD_INFO_OPT_TIME " " U64 " " EG_CMD_INFO_OPT_NODES " " U64 " " EG_CMD_INFO_OPT_SCORE UCI_DELIMITER,
        result->depth, time, engine_atomic_uint64_load(&(result->nodes_explored)));
    if (result->is_mate)
    {
        printf(EG_CMD_INFO_OPT_SCORE_OPT_MATE " " I32 "\n", (int32_t)result->mate_score);
    }
    else
    {
        printf(EG_CMD_INFO_OPT_SCORE_OPT_CP " %.2lf\n", result->centipawns_score);
    }
    fflush(stdout);
}
