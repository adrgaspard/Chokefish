#ifndef SERIALIZATION_TYPES_H
#define SERIALIZATION_TYPES_H

#include "../core/types.h"

typedef struct move_data
{
    position start_pos;
    position dest_pos;
    promotion_type promotion_type;
} move_data;

#endif // SERIALIZATION_TYPES_H
