#include <string.h>
#include "../core/position.h"
#include "consts.h"
#include "position_serializer.h"

static char s_file_names[] = FILE_NAMES;
static char s_rank_names[] = RANK_NAMES;

void position_to_string(position position, char *result)
{
    assert(result && strlen(result) >= POSITION_STR_LEN);
    if (position == NO_POSITION)
    {
        strcpy(result, NO_POSITION_STR);
    }
    else
    {
        result[0] = s_file_names[get_file(position)];
        result[1] = s_rank_names[get_rank(position)];
        result[2] = '\0';
    }
}

position position_from_string(char *string)
{
    assert(string);
    return string[0] >= FIRST_FILE_NAME && string[0] <= LAST_FILE_NAME && string[1] >= FIRST_RANK_NAME && string[1] <= LAST_RANK_NAME
        ? (position)((string[1] - FIRST_RANK_NAME) * RANKS_COUNT + (string[0] - FIRST_FILE_NAME))
        : NO_POSITION;
}