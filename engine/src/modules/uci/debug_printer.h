#ifndef DEBUG_PRINTER_H
#define DEBUG_PRINTER_H

#include "../ai/types.h"
#include "../core/threading.h"

// Creates the persistent debug printing thread, call once before any other function
void initialize_debug_printer(engine_mutex *mutex);

// The following functions must be called while holding the engine mutex

void enable_debug_printing(search_result *result);
void disable_debug_printing(search_result *result, bool print_one_last_time);

#endif // DEBUG_PRINTER_H
