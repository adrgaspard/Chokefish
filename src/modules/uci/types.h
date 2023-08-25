#ifndef UCI_TYPES_H
#define UCI_TYPES_H


typedef enum engine_state
{
    WAITING_FOR_SETUP = 0x01,
    WAITING_FOR_READY = 0x02,
    IDLING = 0x04,
    SEARCHING = 0x08,
    PONDERING = 0x10
} engine_state;

typedef struct engine_options
{
    bool ponder;
    bool own_book;
    int64_t threads_count;
    int64_t threads_count_min;
    int64_t threads_count_max;
} engine_options;

#endif // UCI_TYPES_H
