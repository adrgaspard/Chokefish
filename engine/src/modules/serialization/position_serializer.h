#ifndef POSITION_SERIALIZER_H
#define POSITION_SERIALIZER_H

#include "../core/types.h"

void position_to_string(position position, char *result);
position position_from_string(char *string);

#endif // POSITION_SERIALIZER_H
