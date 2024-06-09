#include "engine_options.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../core/logging.h"

#define _OPTION_PART0 EG_CMD_OPTION UCI_DELIMITER EG_CMD_OPTION_OPT_NAME UCI_DELIMITER
#define _OPTION_PART1 UCI_DELIMITER EG_CMD_OPTION_OPT_TYPE UCI_DELIMITER
#define _OPTION_PART2 UCI_DELIMITER EG_CMD_OPTION_OPT_DEFAULT UCI_DELIMITER

#define _PRINT_BOOL_OPTION(name, value) printf(_OPTION_PART0 name _OPTION_PART1 OPT_BOOL_STR _OPTION_PART2 "%s" "\n", value ? OPT_BOOL_TRUE_STR : OPT_BOOL_FALSE_STR);
#define _PRINT_INT_OPTION(name, value, min, max) printf(_OPTION_PART0 name _OPTION_PART1 OPT_INT_STR _OPTION_PART2 I64 UCI_DELIMITER EG_CMD_OPTION_OPT_MIN UCI_DELIMITER I64 UCI_DELIMITER EG_CMD_OPTION_OPT_MAX UCI_DELIMITER I64 "\n", value, min, max);

#define _TRY_SET_INT_OPTION(option_name_static, option_name_str, option_value_str, option_ptr, value_min, value_max)    \
do                                                                                                                      \
{                                                                                                                       \
    char *convert_ptr;                                                                                                  \
    int64_t convert_result;                                                                                             \
    if (strcmp(option_name_str, option_name_static) == 0)                                                               \
    {                                                                                                                   \
        convert_result = strtol(option_value_str, &convert_ptr, 10);                                                    \
        if (*convert_ptr == '\0' && convert_result >= value_min && convert_result <= value_max)                         \
        {                                                                                                               \
            *option_ptr = convert_result;                                                                               \
        }                                                                                                               \
        return;                                                                                                         \
    }                                                                                                                   \
} while (false);

static void setoption_bool(bool *option_ptr, char *value);
static void setoption_int64(int64_t *option_ptr, char *value, int64_t min, int64_t max);

engine_options get_default_options()
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

bool are_engine_options_valid(engine_options options)
{
    return options.threads_count_min > 0 && options.threads_count >= options.threads_count_min && options.threads_count_max >= options.threads_count;
}

void print_options(engine_options options)
{
    _PRINT_BOOL_OPTION(UCI_OPTION_PONDER, options.ponder)
    _PRINT_BOOL_OPTION(UCI_OPTION_OWNBOOK, options.own_book)
    _PRINT_INT_OPTION(UCI_OPTION_THREADSCOUNT, options.threads_count, options.threads_count_min, options.threads_count_max)
}

void setoption(engine_options *options, char *option_name, char *option_value)
{
    assert(options != NULL);
    assert(option_name != NULL);
    assert(option_value != NULL);
    if (strcmp(option_name, UCI_OPTION_PONDER) == 0)
    {
        setoption_bool(&(options->ponder), option_value);
    }
    else if (strcmp(option_name, UCI_OPTION_OWNBOOK) == 0)
    {
        setoption_bool(&(options->own_book), option_value);
    }
    else if (strcmp(option_name, UCI_OPTION_THREADSCOUNT) == 0)
    {
        setoption_int64(&(options->threads_count), option_value, options->threads_count_min, options->threads_count_max);
    }
}

static void setoption_bool(bool *option_ptr, char *value)
{
    assert(option_ptr != NULL);
    assert(value != NULL);
    if (strcmp(value, OPT_BOOL_TRUE_STR) == 0)
    {
        *option_ptr = true;
    }
    else if (strcmp(value, OPT_BOOL_FALSE_STR) == 0)
    {
        *option_ptr = false;
    } 
}

static void setoption_int64(int64_t *option_ptr, char *value, int64_t min, int64_t max)
{
    char *convertion_ptr;
    int64_t converted_value;
    assert(option_ptr != NULL);
    assert(value != NULL);
    assert(max >= min);
    converted_value = strtol(value, &convertion_ptr, 10);
    if (*convertion_ptr == '\0' && converted_value >= min && converted_value <= max)
    {
        *option_ptr = converted_value;
    }
}