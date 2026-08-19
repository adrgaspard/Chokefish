#include <stdio.h>
#include "../core/move.h"
#include "../serialization/consts.h"
#include "../serialization/move_data_serializer.h"
#include "bestmove_cmd_printer.h"
#include "consts.h"

// The UCI protocol requires a bestmove response for every "go" command, even when the
// engine has nothing to propose: the null move "0000" is used in that case
void print_bestmove_response(search_result *result, bool ponder)
{
    char best_move_str[MOVE_DATA_STR_LEN], ponder_move_str[MOVE_DATA_STR_LEN];
    assert(result != NULL);
    printf(EG_CMD_BESTMOVE " ");
    if (result->valid && !is_movement_empty(result->best_move))
    {
        move_to_string(result->best_move, best_move_str);
        printf("%s", best_move_str);
        if (ponder && !is_movement_empty(result->ponder_move))
        {
            move_to_string(result->ponder_move, ponder_move_str);
            printf(" " EG_CMD_BESTMOVE_OPT_PONDER " %s", ponder_move_str);
        }
    }
    else
    {
        printf(EG_CMD_BESTMOVE_OPT_NULLMOVE);
    }
    printf("\n");
    fflush(stdout);
}
