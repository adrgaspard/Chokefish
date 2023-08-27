#include <pthread.h>
#include <stdio.h>
#include "../core/move.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "bestmove_cmd_printer.h"
#include "consts.h"

void print_bestmove_response(search_result *result, bool ponder)
{
    char best_move_str[MOVE_DATA_STR_LEN], ponder_move_str[MOVE_DATA_STR_LEN];
    pthread_rwlock_rdlock(&(result->lock));
    if (result->valid && !is_movement_empty(result->best_move))
    {
        move_to_string(result->best_move, best_move_str);
        printf(EG_CMD_BESTMOVE " %s", best_move_str);
        if (ponder && !is_movement_empty(result->ponder_move))
        {
            move_to_string(result->ponder_move, ponder_move_str);
            printf(" " EG_CMD_BESTMOVE_OPT_PONDER " %s ", ponder_move_str);
        }
        printf("\n");
    }
    pthread_rwlock_unlock(&(result->lock));
}