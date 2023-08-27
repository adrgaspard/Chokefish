#ifndef DEBUG_PRINTER_H
#define DEBUG_PRINTER_H

#include "../ai/types.h"

void initialize_debug_printer();
void enable_debug_printing(search_result *result);
void disable_debug_printing(search_result *result, bool print_one_last_time);

#endif // DEBUG_PRINTER_H
