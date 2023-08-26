#ifndef SHARED_RANDOM_H
#define SHARED_RANDOM_H

#include <stdint.h>

void initialize_shared_random(uint32_t seed);

uint64_t get_rand_u64();

#endif // SHARED_RANDOM_H
