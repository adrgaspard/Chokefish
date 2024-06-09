#ifndef ENGINE_OPTIONS_H
#define ENGINE_OPTIONS_H

#include "consts.h"
#include "types.h"

#define OPT_NAME_MAX_LENGTH 100
#define OPT_VALUE_MAX_LENGHT 500

#define OPT_BOOL_STR "check"
#define OPT_BOOL_TRUE_STR "true"
#define OPT_BOOL_FALSE_STR "false"
#define OPT_INT_STR "spin"

#define UCI_OPTION_PONDER "Ponder"
#define UCI_OPTION_PONDER_DEFAULT_VALUE true

#define UCI_OPTION_OWNBOOK "OwnBook"
#define UCI_OPTION_OWNBOOK_DEFAULT_VALUE true

#define UCI_OPTION_THREADSCOUNT "Threads"
#define UCI_OPTION_THREADSCOUNT_DEFAULT_VALUE 1
#define UCI_OPTION_THREADSCOUNT_MIN_VALUE 1
#define UCI_OPTION_THREADSCOUNT_MAX_VALUE MAX_WORKER_THREADS

engine_options get_default_options();
bool are_engine_options_valid(engine_options options);
void print_options(engine_options options);
void setoption(engine_options *options, char *option_name, char *option_value);

#endif // ENGINE_OPTIONS_H
