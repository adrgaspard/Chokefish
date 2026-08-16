#ifndef BESTMOVE_CMD_PRINTER_H
#define BESTMOVE_CMD_PRINTER_H

#include "../ai/types.h"

// Must be called while holding the engine mutex
void print_bestmove_response(search_result *result, bool ponder);

#endif // BESTMOVE_CMD_PRINTER_H
