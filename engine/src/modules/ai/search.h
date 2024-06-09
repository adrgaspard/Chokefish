#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

#define MAX_DEPTH 500
#define DRAW_SCORE 0
#define WIN_SCORE 1000000000
#define LOSE_SCORE -(WIN_SCORE)
#define IS_END_SCORE(score) (score > (WIN_SCORE - 10000) || score < (LOSE_SCORE + 10000))

void search_v0(board *board, search_result *search_result, bool *cancellation_requested); 

#endif // SEARCH_H
