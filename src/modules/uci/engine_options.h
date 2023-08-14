#ifndef ENGINE_OPTIONS_H
#define ENGINE_OPTIONS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "consts.h"

#define _OPTION_PART0 EG_CMD_OPTION UCI_DELIMITER EG_CMD_OPTION_OPT_NAME UCI_DELIMITER
#define _OPTION_PART1 UCI_DELIMITER EG_CMD_OPTION_OPT_TYPE UCI_DELIMITER
#define _OPTION_PART2 UCI_DELIMITER EG_CMD_OPTION_OPT_DEFAULT UCI_DELIMITER

#define OPT_BOOL_STR "check"
#define OPT_BOOL_TRUE_STR "true"
#define OPT_BOOL_FALSE_STR "false"
#define OPT_INT_STR "spin"

#define UCI_OPTION_PONDER "Ponder"
#define UCI_OPTION_PONDER_DEFAULT_VALUE true

#define UCI_OPTION_OWNBOOK "OwnBook"
#define UCI_OPTION_OWNBOOK_DEFAULT_VALUE true

#define UCI_OPTION_THREADSCOUNT "ThreadsCount"
#define UCI_OPTION_THREADSCOUNT_DEFAULT_VALUE 1
#define UCI_OPTION_THREADSCOUNT_MIN_VALUE 1
#define UCI_OPTION_THREADSCOUNT_MAX_VALUE MAX_WORKER_THREADS

#define PRINT_BOOL_OPTION(name, value) printf(_OPTION_PART0 name _OPTION_PART1 OPT_BOOL_STR _OPTION_PART2 "%s" "\n", value ? OPT_BOOL_TRUE_STR : OPT_BOOL_FALSE_STR);
#define PRINT_INT_OPTION(name, value, min, max) printf(_OPTION_PART0 name _OPTION_PART1 OPT_INT_STR _OPTION_PART2 "%d" UCI_DELIMITER EG_CMD_OPTION_OPT_MIN UCI_DELIMITER "%d" UCI_DELIMITER EG_CMD_OPTION_OPT_MAX UCI_DELIMITER "%d" "\n", value, min, max);

typedef struct engine_options
{
    bool ponder;
    bool own_book;
    int32_t threads_count;
    int32_t threads_count_min;
    int32_t threads_count_max;
} engine_options;

static inline engine_options get_default_options();
static inline bool are_engine_options_valid(engine_options options);
static inline void print_options(engine_options options);

static inline engine_options get_default_options()
{
    engine_options options;
    options.ponder = UCI_OPTION_PONDER_DEFAULT_VALUE;
    options.own_book = UCI_OPTION_OWNBOOK_DEFAULT_VALUE;
    options.threads_count = UCI_OPTION_THREADSCOUNT_DEFAULT_VALUE;
    options.threads_count_min = UCI_OPTION_THREADSCOUNT_MIN_VALUE;
    options.threads_count_max = UCI_OPTION_THREADSCOUNT_MAX_VALUE;
    assert(are_engine_options_valid(options));
    return options;
}

static inline bool are_engine_options_valid(engine_options options)
{
    return options.threads_count_min > 0 && options.threads_count >= options.threads_count_min && options.threads_count_max >= options.threads_count;
}

static inline void print_options(engine_options options)
{
    PRINT_BOOL_OPTION(UCI_OPTION_PONDER, options.ponder)
    PRINT_BOOL_OPTION(UCI_OPTION_OWNBOOK, options.own_book)
    PRINT_INT_OPTION(UCI_OPTION_THREADSCOUNT, options.threads_count, options.threads_count_min, options.threads_count_max)
}

#endif // ENGINE_OPTIONS_H
