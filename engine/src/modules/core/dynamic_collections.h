#ifndef DYNAMIC_COLLECTIONS_H
#define DYNAMIC_COLLECTIONS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "logging.h"

#define DECLARE_DYNAMIC_ARRAY_CONTRACT(type, name)                              \
typedef struct name                                                             \
{                                                                               \
    type *items;                                                                \
    uint32_t capacity;                                                          \
} name;                                                                         \
                                                                                \
struct name create_##name(uint32_t capacity);                                   \
void finalize_##name(name *array_ptr);


#define DECLARE_DYNAMIC_ARRAY_IMPLEMENTATION(type, name)                        \
struct name create_##name(uint32_t capacity)                                    \
{                                                                               \
    name array;                                                                 \
    assert(capacity > 0);                                                       \
    array.capacity = capacity;                                                  \
    array.items = (type*)calloc((size_t)capacity, sizeof(type));                \
    if (array.items == NULL)                                                    \
    {                                                                           \
        perror("Runtime allocation error on " STRINGIFY_LITERAL(name) "\n");    \
        exit(EXIT_FAILURE);                                                     \
    }                                                                           \
    return array;                                                               \
}                                                                               \
                                                                                \
void finalize_##name(name *array_ptr)                                           \
{                                                                               \
    assert(array_ptr != NULL);                                                  \
    assert(array_ptr != NULL);                                                  \
    free(array_ptr->items);                                                     \
    array_ptr->items = NULL;                                                    \
}

#endif // DYNAMIC_COLLECTIONS_H
