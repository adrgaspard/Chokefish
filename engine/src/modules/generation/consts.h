#ifndef GENERATION_CONSTS_H
#define GENERATION_CONSTS_H

// Partially mutable consts

#define DEFAULT_MAX_BIT_SIZE 13 // Must be greater or equal than 12
#define ITERATIONS_COUNT_PER_SEARCH 1000
#define PRINT_SEARCH_INFORMATIONS_INTERVAL 10000
#define SAVE_SEARCH_RESULTS_INTERVAL 100000
#define FILE_PATH_BASE "generated_magics_"
#define FILE_PATH_MAX_LENGTH 40
#define FILE_PATH_IDENTIFIER_MAX_LENGTH 20 // Must be lower than FILE_PATH_MAX_LENGTH by at least 10
#define PREVIOUS_FILE_PATH "../generated_magics_prev"

#endif // GENERATION_CONSTS_H
