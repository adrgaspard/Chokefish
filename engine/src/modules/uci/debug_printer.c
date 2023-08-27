#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "../ai/types.h"
#include "../core/enhanced_time.h"
#include "../core/logging.h"
#include "consts.h"
#include "debug_printer.h"

#define LOOP_TIME_IN_MS 2000

static pthread_t s_thread;
static bool s_enabled;
static pthread_mutex_t s_mutex;

static void *debug_print_loop(void *arg);
static void print_search_result_info(search_result *result);

void initialize_debug_printer()
{
    s_enabled = false;
    pthread_mutex_init(&s_mutex, NULL);
}

void enable_debug_printing(search_result *result)
{
    pthread_mutex_lock(&s_mutex);
    if (!s_enabled)
    {
        pthread_create(&s_thread, NULL, debug_print_loop, (void *)result);
        s_enabled = true;
    }
    pthread_mutex_unlock(&s_mutex);
}

void disable_debug_printing(search_result *result, bool print_one_last_time)
{
    pthread_mutex_lock(&s_mutex);
    if (s_enabled)
    {
        pthread_cancel(s_thread);
        s_enabled = false;
    }
    pthread_mutex_unlock(&s_mutex);
    if (print_one_last_time)
    {
        assert(result != NULL);
        print_search_result_info(result);
    }
}

static void *debug_print_loop(void *arg)
{
    search_result *result;
    result = (search_result *)arg;
    if (result != NULL)
    {
        usleep(LOOP_TIME_IN_MS * 1000);
        while (true)
        {
            pthread_testcancel();
            print_search_result_info(result);
            usleep(LOOP_TIME_IN_MS * 1000);
        }
    }
    return NULL;
}

static void print_search_result_info(search_result *result)
{
    uint64_t time;
    pthread_rwlock_rdlock(&(result->lock));
    time = get_current_uptime() - result->start_time;
    if (result->valid)
    {
        printf(EG_CMD_INFO UCI_DELIMITER EG_CMD_INFO_OPT_DEPTH  " " U16 " "  EG_CMD_INFO_OPT_TIME " " U64 " " EG_CMD_INFO_OPT_NODES " " U64 " " EG_CMD_INFO_OPT_SCORE UCI_DELIMITER, 
            result->depth, time, result->nodes_explored);
        if (result->is_mate)
        {
            printf(EG_CMD_INFO_OPT_SCORE_OPT_MATE " " U16 "\n", result->mate_score);
        }
        else
        {
            printf(EG_CMD_INFO_OPT_SCORE_OPT_CP " %.2lf\n", result->centipawns_score);
        }
        fflush(stdout);
    }
    pthread_rwlock_unlock(&(result->lock));
}
