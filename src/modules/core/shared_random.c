#include <stdlib.h>
#include "shared_random.h"

void initialize_shared_random(uint32_t seed)
{
    srand(seed);
}

uint64_t get_rand_u64()
{
    return (((uint64_t)(rand() & 0x3F)) << 48) | (((uint64_t)(rand() & 0x3F)) << 32) | (((uint64_t)(rand() & 0x3F)) << 16) | ((uint64_t)(rand() & 0x3F));
}
