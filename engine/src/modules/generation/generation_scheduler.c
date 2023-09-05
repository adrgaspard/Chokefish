#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../core/logging.h"
#include "../core/position.h"
#include "../core/shared_random.h"
#include "consts.h"
#include "magic_generator.h"

static void load_search_results(char *path, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics);
static void save_search_results(char *path, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics);
static void print_search_informations(bool ortho_instead_of_diag, magic_result *best_magics);
static void run_search(magic_generation_data *data, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics, uint32_t search_id);
static void run_search_iterations(magic_generation_data *data, position start_pos, bool ortho_instead_of_diag, uint32_t iterations_count, magic_result *best_magics);

void schedule_generation()
{
    char file_path[FILE_PATH_MAX_LENGTH], identifier[FILE_PATH_IDENTIFIER_MAX_LENGTH];
    magic_generation_data data;
    magic_result orthogonal_best_magics[POSITIONS_COUNT], diagonal_best_magics[POSITIONS_COUNT];
    position pos;
    uint32_t current_search_id;
    uint64_t file_id;
    data = create_magic_generation_data();
    current_search_id = 0;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        orthogonal_best_magics[pos].valid = false;
        orthogonal_best_magics[pos].value = 0;
        orthogonal_best_magics[pos].bit_size = 0;
        diagonal_best_magics[pos].valid = false;
        diagonal_best_magics[pos].value = 0;
        diagonal_best_magics[pos].bit_size = 0;
    }
    load_search_results(PREVIOUS_FILE_PATH, orthogonal_best_magics, diagonal_best_magics);
    while (true)
    {
        if (current_search_id % PRINT_SEARCH_INFORMATIONS_INTERVAL == 0)
        {
            printf("\n------------------------------------------------------\n\n");
            printf(FG_YELLOW "Current time: " I64 "\n\n", time(NULL));
            print_search_informations(true, orthogonal_best_magics);
            printf("\n");
            print_search_informations(false, diagonal_best_magics);
            fflush(stdout);
        }
        if (current_search_id % SAVE_SEARCH_RESULTS_INTERVAL == 0)
        {
            file_id = current_search_id / SAVE_SEARCH_RESULTS_INTERVAL;
            strcpy(file_path, FILE_PATH_BASE);
            sprintf(identifier, U64, file_id);
            strcat(file_path, identifier);
            save_search_results(file_path, orthogonal_best_magics, diagonal_best_magics);
        }
        run_search(&data, orthogonal_best_magics, diagonal_best_magics, ++current_search_id);
    }
}

static void load_search_results(char *path, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics)
{
    position pos;
    FILE *buffer;
    magic_result result;
    assert(orthogonal_best_magics != NULL);
    assert(diagonal_best_magics != NULL);
    if (path == NULL)
    {
        return;
    }
    buffer = fopen(path, "r");
    if (buffer != NULL)
    {
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            if (fscanf(buffer, U64 " " U8, &(result.value), &(result.bit_size)) == EOF)
            {
                perror("File reading error");
                exit(1);
            }
            result.valid = result.value > 0 && result.bit_size > 0;
            if (is_new_magic_result_better(orthogonal_best_magics[pos], result))
            {
                orthogonal_best_magics[pos] = result;
            }
        }
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            if (fscanf(buffer, U64 " " U8, &(result.value), &(result.bit_size)) == EOF)
            {
                perror("File reading error");
                exit(1);
            }
            result.valid = result.value > 0 && result.bit_size > 0;
            if (is_new_magic_result_better(diagonal_best_magics[pos], result))
            {
                diagonal_best_magics[pos] = result;
            }
        }
        fclose(buffer);
    }
}

static void save_search_results(char *path, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics)
{
    position pos;
    FILE *buffer;
    assert(orthogonal_best_magics != NULL);
    assert(diagonal_best_magics != NULL);
    if (path == NULL)
    {
        return;
    }
    buffer = fopen(path, "w");
    if (buffer != NULL)
    {
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            fprintf(buffer, U64 " " U16 "\n", orthogonal_best_magics[pos].value, orthogonal_best_magics[pos].bit_size);
        }
        fprintf(buffer, "\n");
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            fprintf(buffer, U64 " " U16 "\n", diagonal_best_magics[pos].value, diagonal_best_magics[pos].bit_size);
        }
        fclose(buffer);
    }
}

static void print_search_informations(bool ortho_instead_of_diag, magic_result *best_magics)
{
    position pos;
    uint8_t magics_found_count, min_bit_size, max_bit_size;
    double total_kb_size;
    assert(best_magics != NULL);
    magics_found_count = 0;
    min_bit_size = UINT8_MAX;
    max_bit_size = 0;
    total_kb_size = 0;
    for (pos = 0; pos < POSITIONS_COUNT; pos++)
    {
        if (best_magics[pos].valid)
        {
            magics_found_count++;
            if (min_bit_size > best_magics[pos].bit_size)
            {
                min_bit_size = best_magics[pos].bit_size;
            }
            if (max_bit_size < best_magics[pos].bit_size)
            {
                max_bit_size = best_magics[pos].bit_size;
            }
            total_kb_size += ((1 << best_magics[pos].bit_size) * 8 / (double)1000);
        }
    }
    printf(FG_WHITE "%s ", ortho_instead_of_diag ? "Orthogonal magics" : "Diagonal magics");
    printf("%s" "%02d / %02d found" COLOR_RESET "\n", magics_found_count == POSITIONS_COUNT ? BG_GREEN : BG_RED, magics_found_count, POSITIONS_COUNT);
    printf(FG_GRAY "Lowest required bit count: " FG_WHITE U8 COLOR_RESET "\n", min_bit_size);
    printf(FG_GRAY "Highest required bit count: " FG_WHITE U8 COLOR_RESET "\n", max_bit_size);
    printf(FG_GRAY "Average size per position: " FG_WHITE "%.2f" FG_GRAY " kb" COLOR_RESET "\n", total_kb_size / (magics_found_count > 0 ? magics_found_count : 1));
    printf(FG_GRAY "Total size: " FG_WHITE "%.2f" FG_GRAY " kb" COLOR_RESET "\n", total_kb_size);
    fflush(stdout);
}

static void run_search(magic_generation_data *data, magic_result *orthogonal_best_magics, magic_result *diagonal_best_magics, uint32_t search_id)
{
    position remaining_pos[POSITIONS_COUNT];
    position remaining_pos_count;
    magic_result *best_magics, worst_magic;
    position pos, iterator, worst_pos, current_index, current_pos;
    bool searching_worst_pos;
    bool orthogonal_search;
    assert(data != NULL);
    assert(orthogonal_best_magics != NULL);
    assert(diagonal_best_magics != NULL);
    searching_worst_pos = get_rand_u64() % 10 < 8;
    if (searching_worst_pos)
    {
        orthogonal_search = search_id % 2 == 0;
        best_magics = orthogonal_search ? orthogonal_best_magics : diagonal_best_magics;
        for (pos = 0; pos < POSITIONS_COUNT; pos++)
        {
            remaining_pos[pos] = pos;
        }
        remaining_pos_count = POSITIONS_COUNT;
        worst_pos = 0;
        worst_magic = best_magics[0];
        for (iterator = 0; iterator < POSITIONS_COUNT; iterator++)
        {
            current_index = (position)(get_rand_u64() % (uint8_t)remaining_pos_count);
            remaining_pos_count--;
            current_pos = remaining_pos[current_index];
            remaining_pos[current_index] = remaining_pos[remaining_pos_count];
            if (!best_magics[current_pos].valid || (best_magics[current_pos].bit_size > worst_magic.bit_size && worst_magic.valid))
            {
                worst_magic = best_magics[current_pos];
                worst_pos = current_pos;
            }
        }
        run_search_iterations(data, worst_pos, orthogonal_search, DEFAULT_MAX_BIT_SIZE, best_magics);
    }
    else
    {
        run_search_iterations(data, (position)(get_rand_u64() % POSITIONS_COUNT), true, ITERATIONS_COUNT_PER_SEARCH, orthogonal_best_magics);
        run_search_iterations(data, (position)(get_rand_u64() % POSITIONS_COUNT), false, ITERATIONS_COUNT_PER_SEARCH, diagonal_best_magics);
    }
}

static void run_search_iterations(magic_generation_data *data, position start_pos, bool ortho_instead_of_diag, uint32_t iterations_count, magic_result *best_magics)
{
    uint8_t max_bit_size;
    magic_result new_magic;
    assert(data != NULL);
    assert(is_position_valid(start_pos) && start_pos != NO_POSITION);
    assert(best_magics != NULL);
    max_bit_size = (uint8_t)(best_magics[start_pos].valid ? best_magics[start_pos].bit_size - 1 : DEFAULT_MAX_BIT_SIZE);
    new_magic = compute_magic(data, start_pos, ortho_instead_of_diag, iterations_count, max_bit_size);
    if (is_new_magic_result_better(best_magics[start_pos], new_magic))
    {
        best_magics[start_pos] = new_magic;
    }
}