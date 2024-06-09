#ifndef MAGIC_GENERATOR_H
#define MAGIC_GENERATOR_H

#include "types.h"

magic_generation_data create_magic_generation_data();
magic_result compute_magic(magic_generation_data *data, position start_pos, bool ortho_instead_of_diag, uint32_t iterations_count, uint8_t max_bit_size);
bool is_new_magic_result_better(magic_result old_result, magic_result new_result);

#endif // MAGIC_GENERATOR_H
